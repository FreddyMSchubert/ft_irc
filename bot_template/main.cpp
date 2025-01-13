#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <tuple>
#include <unistd.h>
#include <arpa/inet.h>
#include "Bot.hpp"

void onMessage(std::string user, std::string channel, std::string message);
void onDisconnect();
void onConnect();
void onError(std::string message);

Bot &getBot()
{
	static Bot bot = Bot();

	return bot;
}

// TODO: fix that callbacks are not being called correctly
int main(int argc, char *argv[])
{
	try
	{
		Bot &bot = getBot();

		bot.setCallbacks(onConnect, onError, onMessage, onDisconnect);
		bot.setIp("127.0.0.1");
		bot.setPort(6667);
		bot.setNick("bot");
		bot.setUser("bot");
		bot.setPassword("password");

		bot.connectToServer();

		// bot.authenticate();
		// bot.startPollingForEvents();
		// bot.changeChannel("#bot");

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
		getBot().changeChannel("#bot");
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
	std::cout << "User:\t\t" << user << "\n";
	std::cout << "Channel:\t\t" << channel << "\n";
	std::cout << "Message:\t\t" << message << std::endl;

	if (channel != "#bot")
	{
		getBot().directMessage(user, "Please send messages to #bot channel only.");
		return;
	}

	if (message == "ping")
		getBot().directMessage(user, "pong");

	// getBot().directMessage(user, "Hello! You said: " + message);
}

// custom function to handle disconnect
void onDisconnect()
{
	std::cerr << "Disconnected from server!" << std::endl;
}
