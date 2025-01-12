#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include "Bot.hpp"
#include "./bot_dadjokes/bot_dadjokes.hpp"
#include "./bot_chatgpt/bot_chatgpt.hpp"
#include "./bot_tictactoe/bot_tictactoe.hpp"

void onMessage(std::string user, std::string channel, std::string message);
void onDisconnect();
void onConnect();
void onError(std::string message);

Bot &getBot()
{
	static Bot bot("127.0.0.1", 6667, "password", "bot", "template_bot");

	return bot;
}

int main(int argc, char *argv[])
{
	try
	{
		if (strcmp(argv[6], "dadjokes"))
		{
			Bot_DadJokes botDadJokes(
				std::string(argv[1]),
				std::atoi(argv[2]),
				std::string(argv[3]),
				std::string(argv[4]),
				std::string(argv[5])
			);

			botDadJokes.setCallbacks(onConnect, onError, onMessage, onDisconnect);
			botDadJokes.connectToServer();
			botDadJokes.authenticate();
			botDadJokes.sendMessage(botDadJokes.ApiCall());
		}
		else if (strcmp(argv[6], "chatgpt") && argv[7] != nullptr)
		{
			Bot_ChatGPT botChatGPT(
				std::string(argv[1]),
				std::atoi(argv[2]),
				std::string(argv[3]),
				std::string(argv[4]),
				std::string(argv[5])
			);

			botChatGPT.setCallbacks(onConnect, onError, onMessage, onDisconnect);
			botChatGPT.connectToServer();
			botChatGPT.authenticate();
			botChatGPT.sendMessage(botChatGPT.ApiCall(argv[7]));
		}
		else if (strcmp(argv[6], "MSG") || argv[6][0] == '#')
		{
			// Bot_TicTacToe botTicTacToe(
			// 	std::string(argv[1]),
			// 	std::atoi(argv[2]),
			// 	std::string(argv[3]),
			// 	std::string(argv[4]),
			// 	std::string(argv[5]),
			// 	std::string(argv[6])
			// );

			// botTicTacToe.setCallbacks(onConnect, onError, onMessage, onDisconnect);
			// botTicTacToe.connectToServer();
			// botTicTacToe.authenticate();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

// custom function to handle connection
void onConnect()
{
	std::cout << "Connected to IRC Server!" << std::endl;
	try {
		std::cout << "Trying to athenticate." << std::endl;
		getBot().authenticate();
		std::cout << "Authenticating passed!" << std::endl;
		getBot().startPollingForEvents();
	} catch (std::exception& e) {
		std::cerr << "Failed to authenticate: " << e.what() << std::endl;
	}
}

// custom function to handle errors
void onError(std::string message)
{
	std::cerr << "ERROR: " << message << std::endl;
}

// custom function to handle messages
void onMessage(std::string user, std::string channel, std::string message)
{
	std::cout << "Message received:\n";
	std::cout << "\tUser: " << user << "\n";
	std::cout << "\tChannel: " << channel << "\n";
	std::cout << "\tMessage: " << message << std::endl;

	if (channel == "ping")
		getBot().sendMessage(user.substr(0, user.length() - 1), "pong");

	// getBot().directMessage(user, "Hello! You said: " + message);
}

// custom function to handle disconnect
void onDisconnect()
{
	std::cerr << "Disconnected from server!" << std::endl;
}