#pragma once

#include "Socket.hpp"
#include "./Utils/Logger.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <chrono>

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
	std::string buffer;

	s_socket_data(int fd, Socket socket) // Client socket constructor
		: socket(std::move(socket)), fd(fd), buffer() {}
	s_socket_data(int port) // Listening socket constructor
		: socket(port), fd(socket.getSocketFd()) {}
	s_socket_data(const s_socket_data&) = delete;
	s_socket_data& operator=(const s_socket_data&) = delete;

	s_socket_data(s_socket_data&& other) noexcept
		: socket(std::move(other.socket)), fd(other.fd), states(other.states), buffer(std::move(other.buffer)) {}
	s_socket_data& operator=(s_socket_data&& other) noexcept
	{
		if (this == &other)
			return *this;
		socket = std::move(other.socket);
		fd = other.fd;
		states = other.states;
		buffer = other.buffer;
		return *this;
	}
}	t_socket_data;

class ConnectionManager
{
	private:
		int							_port;
		t_socket_data				_listening_socket;
		std::vector<t_socket_data>	_sockets;

		void updatePoll();
		void acceptNewConnections();
		void handleExistingConnections();

	public:
		ConnectionManager(int port);
		ConnectionManager(ConnectionManager const &src) = delete;
		ConnectionManager &operator=(ConnectionManager const &src) = delete;
		ConnectionManager(ConnectionManager&& other) noexcept = default;
		~ConnectionManager() = default;
		
		void Run(); // Main ConnectionManager loop
};
