#include "Bot.hpp"
#include <exception>
#include <stdexcept>
#include <unistd.h>

Bot::Bot(std::string ip, int port, std::string password, std::string nick, std::string user)
{
	if (ip.empty() || port <= 0)
	{
		std::cerr << "Error: IP or port not set" << std::endl;
		return;
	}

	if (password.empty())
	{
		std::cerr << "Error: Password not set" << std::endl;
		return;
	}

	_ip = ip;
	_port = port;
	_password = password;
	if (!nick.empty())
		_nick = nick;
	if (!user.empty())
		_user = user;

	socket.addEventListener(EventType::ON_CONNECT, std::bind(&Bot::onConnect, this, std::placeholders::_1));
	socket.addEventListener(EventType::ON_ERROR, std::bind(&Bot::onError, this, std::placeholders::_1));
	socket.addEventListener(EventType::ON_MESSAGE, std::bind(&Bot::onMessage, this, std::placeholders::_1));
	socket.addEventListener(EventType::ON_DISCONNECT, std::bind(&Bot::onDisconnect, this, std::placeholders::_1));
}

Bot::~Bot()
{
	socket.queueMessage("QUIT");
}

void Bot::connectToServer()
{
	try {
		socket.connectToServer(this->_ip, this->_port);
	} catch (std::exception& e) {
		std::cerr << "Socket: Failed to connect to IRC Server: " << e.what() << std::endl;
		throw std::runtime_error("Failed to connect to IRC Server");
	} 
}

void Bot::authenticate()
{
	std::cout << "Authenticating with:" << std::endl;
	std::cout << "\tPASS: " << _password << std::endl;
	std::cout << "\tNICK: " << _nick << std::endl;
	std::cout << "\tUSER: " << _user << std::endl;
	sendRawMessage("PASS " + _password);
	sleep(1);
	sendRawMessage("NICK " + _nick);
	sleep(1);
	sendRawMessage("USER bot" + _user);
	sleep(1);
}

void Bot::directMessage(std::string user, std::string msg)
{
	socket.queueMessage("PRIVMSG " + user + " :" + msg);
}

void Bot::sendRawMessage(std::string msg)
{
	socket.queueMessage(msg);
}

void Bot::sendMessage(std::string msg)
{
	socket.queueMessage("PRIVMSG " + _current_channel + " :" + msg);
}

void Bot::sendMessage(std::string channelname, std::string msg)
{
	socket.queueMessage("PRIVMSG " + channelname + " :" + msg);
}

void Bot::changeChannel(std::string channel)
{
	_current_channel = channel;
	socket.queueMessage("JOIN " + channel);
}

void Bot::changeChannel(std::string channel, std::string password)
{
	_current_channel = channel;
	socket.queueMessage("JOIN " + channel + " " + password);
}

void Bot::onConnect(std::string message)
{
	(void)message;
	std::cout << "Connected to IRC Server\n" << std::endl;
	try {
		this->authenticate();
	} catch (std::exception& e) {
		std::cerr << "Failed to authenticate: " << e.what() << std::endl;
	}
	socket.Run();
}

void Bot::onError(std::string message)
{
	std::cerr << "Error: " << message << std::endl;
}

void Bot::onMessage(std::string message)
{
	std::cout << "Message received: " << message << std::endl;
	socket.queueMessage("Hello! You said: " + message);
}

void Bot::onDisconnect(std::string message)
{
	std::cerr << "Disconnected from server: " << message << std::endl;
}