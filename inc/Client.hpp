#pragma once

#include "Socket.hpp"

typedef struct s_socket_state
{
	bool read;			// POLLIN
	bool write;			// POLLOUT
	bool disconnect;	// POLLHUP
	bool error;			// POLLERR
}	t_socket_state;

class Client
{
	public:
		Socket			socket;
		int				fd;
		t_socket_state	states;
		std::string		inbuffer;
		std::string		outbuffer;

		Client(int fd, Socket socket);	// Client socket constructor
		Client(int port);				// Listening socket constructor
		Client(const Client&) = delete;
		Client& operator=(const Client&) = delete;
		Client(Client&& other) noexcept;
		Client& operator=(Client&& other) noexcept;
};
