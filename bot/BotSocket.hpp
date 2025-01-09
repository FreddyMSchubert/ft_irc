#ifndef BOTSOCKET_HPP
#define BOTSOCKET_HPP

#include <map>
#include <string>
#include <vector>
#include <poll.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class BotSocket
{
	private:
		int							_listener;
		std::vector<struct pollfd>	_pollFds;

	public:
		void setupListener();

		//Getter
		int getListenerFd() { return _listener;}
};

#endif