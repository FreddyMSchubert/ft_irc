#pragma once

#include <iostream>

enum class EventType
{
	ON_MESSAGE,
	ON_DICONNECT,
	ON_CONNECT
};

class Socket
{
	private:
		using EventCallback = void(*)(std::string message);
		struct Event
		{
			EventType type;
			EventCallback callback;
		};

		std::string				_socket_ip;
		int						_socket_port;
		std::vector<Event>()	_events;

		void executeEventsOfType(EventType type);

	public:
		Socket();
		~Socket();

		void addEventListener();
		void sendMessage(std::string msg);

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
		Bot(std::string ip, int port, std::string password);
		~Bot();

		void authenticate();
		void directMessage(std::string user, std::string msg);
		void sendMessage(std::string msg);
		void sendMessage(std::string msg, std::string channelname);
		void changeChannel(std::string channelname);
		void changeChannel(std::string channelname, std::string password);
};
