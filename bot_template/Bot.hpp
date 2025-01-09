#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include <cstring>
#include <algorithm>
#include <exception>
#include <string>
#include <functional>

class Bot;

typedef std::function<void(std::string, std::string, std::string, Bot&)> onMessageCallback; //channel, user, message, botRef
typedef std::function<void(Bot&)> onDisconnectCallback;
typedef std::function<void(Bot&)> onConnectCallback;
typedef std::function<void(std::string, Bot&)> onErrorCallback;

class Socket
{
	private:
		onDisconnectCallback		_onDisconnectCallback = nullptr;
		onMessageCallback			_onMessageCallback = nullptr;
		onConnectCallback			_onConnectCallback = nullptr;
		onErrorCallback				_onErrorCallback = nullptr;

		std::string					_socket_ip;
		int							_socket_port;
		std::vector<std::string>	_messages;

		Bot &_botRef;

		int _socket_fd = -1;
		struct sockaddr_in _socket;
		bool running = true;
		bool error = false;

		void setNonBlocking();
		void sendMessage(std::string msg);

	public:
		Socket(Bot &bot);
		~Socket();

		void connectToServer(std::string ip, int port);

		void setOnMessageCallback(onMessageCallback callback) { _onMessageCallback = callback; };
		void setOnDisconnectCallback(onDisconnectCallback callback) { _onDisconnectCallback = callback; };
		void setOnConnectCallback(onConnectCallback callback) { _onConnectCallback = callback; };
		void setOnErrorCallback(onErrorCallback callback) { _onErrorCallback = callback; };

		void queueMessage(std::string msg);

		void Run();

};

class Bot
{
	private:
		std::string	_ip = "127.0.0.1";
		int			_port = 6667;
		std::string _password;
		std::string	_current_channel;

		//bot
		std::string	_nick = "bot";
		std::string	_user = "bot";

		//socket
		Socket *socket = nullptr;

	public:
		Bot(std::string ip, int port, std::string password, std::string nick, std::string user);
		~Bot();

		void connectToServer();
		void authenticate();
		void sendRawMessage(std::string msg);
		void directMessage(std::string user, std::string msg);
		void sendMessage(std::string msg);
		void sendMessage(std::string channelname, std::string msg);
		void changeChannel(std::string channelname);
		void changeChannel(std::string channelname, std::string password);

		void setCallbacks(onConnectCallback onConnect,
					onErrorCallback onError,
					onMessageCallback onMessage,
					onDisconnectCallback onDisconnect);
};
