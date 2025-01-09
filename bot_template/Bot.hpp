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

enum class EventType
{
	NONE,
	ON_MESSAGE,
	ON_DISCONNECT,
	ON_CONNECT,
	ON_ERROR
};

class Socket
{
	private:
		typedef std::function<void(std::string)> EventCallback;
		struct Event
		{
			EventType type;
			EventCallback callback;
		};

		std::string				_socket_ip;
		int						_socket_port;
		std::vector<Event>		_events;
		std::vector<std::string> _messages;

		int _socket_fd = -1;
		struct sockaddr_in _socket;
		bool running = true;

		void setNonBlocking();
		void executeEventsOfType(EventType type, std::string message);
		void sendMessage(std::string msg);
	public:
		Socket();
		~Socket();

		void connectToServer(std::string ip, int port);

		void addEventListener(EventType type, EventCallback callback);
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
		Socket socket;

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

		void onMessage(std::string message);
		void onDisconnect(std::string message);
		void onConnect(std::string message);
		void onError(std::string message);
};
