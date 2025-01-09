#include "Bot.hpp"
#include <exception>
#include <stdexcept>
#include <unistd.h>

void onMessage(std::string user, std::string channel, std::string message, Bot& bot);
void onDisconnect(std::string reason, Bot& bot);
void onConnect(std::string server, Bot &bot);
void onError(std::string message, Bot &bot);

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
	// socket.queueMessage("QUIT");
	if (socket)
		delete socket;
}

void Bot::connectToServer()
{
	socket = new Socket(*this);

	if (!socket)
		throw std::runtime_error("Failed to create socket");

	try {
		socket->connectToServer(this->_ip, this->_port);
	} catch (std::exception& e) {
		throw std::runtime_error(std::string(e.what()));
	} 
}

void Bot::authenticate()
{
	std::cout << "Authenticating with:" << std::endl;
	std::cout << "\tPASS: " << _password << std::endl;
	std::cout << "\tNICK: " << _nick << std::endl;
	std::cout << "\tUSER: " << _user << std::endl;

	sendRawMessage("PASS " + _password + "\r\n");
	sendRawMessage("NICK " + _nick + "\r\n");
	sendRawMessage("USER bot" + _user+ "\r\n");
}

void Bot::directMessage(std::string user, std::string msg)
{
	socket->queueMessage("PRIVMSG " + user + " :" + msg);
}

void Bot::sendRawMessage(std::string msg)
{
	socket->queueMessage(msg);
}

void Bot::sendMessage(std::string msg)
{
	socket->queueMessage("PRIVMSG " + _current_channel + " :" + msg);
}

void Bot::sendMessage(std::string channelname, std::string msg)
{
	socket->queueMessage("PRIVMSG " + channelname + " :" + msg);
}

void Bot::changeChannel(std::string channel)
{
	_current_channel = channel;
	socket->queueMessage("JOIN " + channel);
}

void Bot::changeChannel(std::string channel, std::string password)
{
	_current_channel = channel;
	socket->queueMessage("JOIN " + channel + " " + password);
}

void Bot::setCallbacks(onConnectCallback onConnect,
					onErrorCallback onError,
					onMessageCallback onMessage,
					onDisconnectCallback onDisconnect)
{
	socket->setOnMessageCallback(onMessage);
	socket->setOnDisconnectCallback(onDisconnect);
	socket->setOnConnectCallback(onConnect);
	socket->setOnErrorCallback(onError);
}