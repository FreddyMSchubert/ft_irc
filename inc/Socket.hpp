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

#include "./Utils/Logger.hpp"

class Socket
{
private:
	int _socket_fd = -1;
	struct sockaddr_in _socket;
	int _port;

public:
	// Existing constructors
	Socket(int port); // Listening socket constructor
	Socket(int fd, int port); // Client socket constructor
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	~Socket();

	Socket(Socket&& other) noexcept;
	Socket& operator=(Socket&& other) noexcept;

	int getSocketFd() const { return _socket_fd; }
	std::string receiveData();
	void sendData(const std::string & data);

	void setNonBlocking();
};
