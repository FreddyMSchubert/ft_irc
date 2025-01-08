#include "../inc/Client.hpp"

Client::Client(int fd, Socket socket) // Client socket constructor
	: socket(std::move(socket)), fd(fd) {}
Client::Client(int port) // Listening socket constructor
	: socket(port), fd(socket.getSocketFd()) {}

Client::Client(Client&& other) noexcept
	: socket(std::move(other.socket)), fd(other.fd), states(other.states), inbuffer(std::move(other.inbuffer)) {}
Client& Client::operator=(Client&& other) noexcept
{
	if (this == &other)
		return *this;
	socket = std::move(other.socket);
	fd = other.fd;
	states = other.states;
	inbuffer = other.inbuffer;
	return *this;
}

std::string Client::getName()
{
	if (!nickname.empty())
		return nickname;
	if (!username.empty())
		return username;
	return std::to_string(fd);
}
