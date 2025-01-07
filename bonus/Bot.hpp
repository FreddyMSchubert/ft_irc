#ifndef BOT_HPP
#define BOT_HPP

#include <string>

class Server;

class Bot
{
public:
	Bot() {}
	~Bot() {}

	void handleBotMessage(Server &srv, int fd, const std::string &message);
};

#endif
