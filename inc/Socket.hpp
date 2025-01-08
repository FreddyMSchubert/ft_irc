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

#include "Logger.hpp"

class Socket
{
private:
	int m_socket_fd = -1;
	struct sockaddr_in m_socket;
	bool m_is_error_response = false;
	std::string m_server_ip;
	int m_server_port;

public:
	Socket(std::string server_ip, int server_port);
	Socket(int fd);
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	~Socket();

	Socket(Socket&& other) noexcept;
	Socket& operator=(Socket&& other) noexcept;

	int getSocketFd() const { return m_socket_fd; }
	std::string receiveData();
	void sendData(std::string data);

	void setNonBlocking();
};