#include "Bot.hpp"
#include <exception>
#include <stdexcept>
#include <unistd.h>

void onMessage(std::string user, std::string channel, std::string message);
void onDisconnect(std::string reason);
void onConnect(std::string server);
void onError(std::string message);

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
}

void Bot::connectToServer()
{
	try {
		socket.connectToServer(this->_ip, this->_port);
	} catch (std::exception& e) {
		throw std::runtime_error(std::string(e.what()));
	} 
}

void Bot::authenticate()
{
	socket.queueMessage("NICK " + _nick);
	socket.queueMessage("USER bot_" + _user);
	socket.queueMessage("PASS " + _password);
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
	socket.queueMessage("PRIVMSG " + _current_channel + " " + msg);

}

void Bot::sendMessage(std::string channelname, std::string msg)
{
	socket.queueMessage("PRIVMSG " + channelname + " :" + msg);
}

void Bot::changeChannel(std::string channel)
{
	socket.queueMessage("JOIN " + channel);
	_current_channel = channel;
}

void Bot::changeChannel(std::string channel, std::string password)
{
	socket.queueMessage("JOIN " + channel + " " + password);
	_current_channel = channel;
}

void Bot::setCallbacks(onConnectCallback onConnect,
					onErrorCallback onError,
					onMessageCallback onMessage,
					onDisconnectCallback onDisconnect)
{
	socket.setOnMessageCallback(onMessage);
	socket.setOnDisconnectCallback(onDisconnect);
	socket.setOnConnectCallback(onConnect);
	socket.setOnErrorCallback(onError);
}

void Bot::startPollingForEvents()
{
	socket.Run();
}