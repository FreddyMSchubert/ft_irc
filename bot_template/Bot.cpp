#include "Bot.hpp"
#include <exception>
#include <stdexcept>

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
}

Bot::~Bot()
{
	socket.sendMessage("QUIT");
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
	socket.sendMessage("PASS " + _password);
	socket.sendMessage("NICK " + _nick);
	socket.sendMessage("USER bot" + _user);
}

void Bot::directMessage(std::string user, std::string msg)
{
	socket.sendMessage("PRIVMSG " + user + " :" + msg);
}

void Bot::sendMessage(std::string msg)
{
	socket.sendMessage("PRIVMSG " + _current_channel + " :" + msg);
}

void Bot::sendMessage(std::string channelname, std::string msg)
{
	socket.sendMessage("PRIVMSG " + channelname + " :" + msg);
}

void Bot::changeChannel(std::string channel)
{
	_current_channel = channel;
	socket.sendMessage("JOIN " + channel);
}

void Bot::changeChannel(std::string channel, std::string password)
{
	_current_channel = channel;
	socket.sendMessage("JOIN " + channel + " " + password);
}

void Bot::onConnect(std::string message)
{
	(void)message;
	std::cout << "Connected to IRC Server\nAuthenticating..." << std::endl;
	try {
		this->authenticate();
	} catch (std::exception& e) {
		std::cerr << "Failed to authenticate: " << e.what() << std::endl;
	}
}

void Bot::onError(std::string message)
{
	std::cerr << "Error: " << message << std::endl;
}

void Bot::onMessage(std::string message)
{
	std::cout << "Message received: " << message << std::endl;
	socket.sendMessage("Hello! You said: " + message);
}

void Bot::Run()
{
	socket.Run();
}