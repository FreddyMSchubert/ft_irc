#ifndef BOTSERVER_HPP
#define BOTSERVER_HPP

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <vector>
#include <poll.h>

class BotServer
{
	private:

	public:
		void run();
};

#endif