#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include <cstring>
#include <algorithm>
#include <exception>
#include <string>
#include <map>
#include <fstream>
#include <array>
#include <chrono>
#include <sstream>

#include "./Socket.hpp"

// class Channel;

typedef struct s_socket_state
{
	bool read; // POLLIN
	bool write; // POLLOUT
	bool disconnect; // POLLHUP
	bool error; // POLLERR
}	t_socket_state;

typedef struct s_socket_data
{
	Socket socket;
	int fd;
	t_socket_state states;
	std::stringstream buffer;

	s_socket_data(int fd, Socket socket) : socket(std::move(socket)), fd(fd), buffer() {}
	s_socket_data(const s_socket_data&) = delete;
	s_socket_data& operator=(const s_socket_data&) = delete;

	s_socket_data(s_socket_data&& other) noexcept : socket(std::move(other.socket)), fd(other.fd), states(other.states), buffer(std::move(other.buffer)) {}
	s_socket_data& operator=(s_socket_data&& other) noexcept
	{
		if (this == &other)
			return *this;
		socket = std::move(other.socket);
		fd = other.fd;
		states = other.states;
		buffer = std::move(other.buffer);
		return *this;
	}
}	t_socket_data;

class Server
{
	private:
		t_socket_data					m_listening_socket;
		std::vector<t_socket_data>		m_sockets;
		// std::map<std::string, Channel> 	m_channels;
		std::string						m_ip;
		int								m_port;
		std::string						m_password;

		void updatePoll();
		void acceptNewConnections();
		void handleExistingConnections();

	public:
		Server(std::string ip, int port, std::string password);
		Server(Server const &src) = delete;
		Server &operator=(Server const &src) = delete;
		Server(Server&& other) noexcept = default;
		~Server() = default;
		
		void Run(); // Main server loop
};