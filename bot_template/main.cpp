#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include "Bot.hpp"

void onMessage(std::string user, std::string channel, std::string message, Bot& bot);
void onDisconnect(Bot& bot);
void onConnect(Bot &bot);
void onError(std::string message, Bot &bot);

int main(int argc, char *argv[])
{
    Bot bot("127.0.0.1", 6667, "password", "bot", "template_bot");

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
void onConnect(Bot &bot)
{
	std::cout << "Connected to IRC Server!" << std::endl;
	try {
		bot.authenticate();
	} catch (std::exception& e) {
		std::cerr << "Failed to authenticate: " << e.what() << std::endl;
	}
}

// custom function to handle errors
void onError(std::string message, Bot &bot)
{
    (void)bot;
	std::cerr << "ERROR: " << message << std::endl;
}

// custom function to handle messages
void onMessage(std::string user, std::string channel, std::string message, Bot &bot)
{
	std::cout << "Message received:\n";
	std::cout << "\tUser: " << user << "\n";
	std::cout << "\tChannel: " << channel << "\n";
	std::cout << "\tMessage: " << message << std::endl;

	bot.directMessage(user, "Hello! You said: " + message);
}

// custom function to handle disconnect
void onDisconnect(Bot &bot)
{
    (void)bot;
	std::cerr << "Disconnected from server: " << std::endl;
}