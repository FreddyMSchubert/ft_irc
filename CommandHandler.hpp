#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>

class Server;

class CommandHandler
{
public:
	CommandHandler() {}
	~CommandHandler() {}

	void handleCommand(Server &srv, int fd, const std::string &line);
};

#endif
