#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include "Bot.hpp"

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
    
	Bot &bot = getBot();
    bot.setCallbacks(onConnect, onError, onMessage, onDisconnect);

    try {
        bot.connectToServer();
    } catch (std::exception& e) {
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

	if (message == "ping")
		getBot().sendMessage(channel, "pong");

	// getBot().directMessage(user, "Hello! You said: " + message);
}

// custom function to handle disconnect
void onDisconnect()
{
	std::cerr << "Disconnected from server: " << std::endl;
}