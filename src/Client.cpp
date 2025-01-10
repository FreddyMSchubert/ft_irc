#include "../inc/Client.hpp"

#include "../inc/Channel.hpp" // avoid circ depend

Client::Client(int fd, Socket socket, int id) // Client socket constructor
	: id(id), socket(std::move(socket)), fd(fd) {}
Client::Client(int port) // Listening socket constructor
	: id(-1), socket(port), fd(socket.getSocketFd()) {}

Client::Client(Client&& other) noexcept :
	id(other.id),
	socket(std::move(other.socket)),
	fd(other.fd),
	states(other.states),
	inbuffer(std::move(other.inbuffer)),
	outbuffer(std::move(other.outbuffer)),
	channel(other.channel),
	isAuthenticated(other.isAuthenticated),
	isOperator(other.isOperator),
	nickname(other.nickname),
	username(other.username)
	{}
Client& Client::operator=(Client&& other) noexcept
{
	if (this == &other)
		return *this;
	id = other.id;
	socket = std::move(other.socket);
	fd = other.fd;
	states = other.states;
	inbuffer = other.inbuffer;
	outbuffer = other.outbuffer;
	channel = other.channel;
	isAuthenticated = other.isAuthenticated;
	isOperator = other.isOperator;
	nickname = other.nickname;
	username = other.username;
	return *this;
}

std::string Client::getName()
{
	if (!nickname.empty())
		return nickname;
	if (!username.empty())
		return username;
	return std::string("User_") + std::to_string(id);
}

bool Client::isOperatorIn(Channel * channel)
{
	if (!channel)
		return isOperator;
	return isOperator || channel->isOperator(id);
}

void Client::sendMessage(std::string msg)
{
	if (msg.empty())
		return;
	if (msg[msg.size() - 1] != '\n')
		msg += "\r\n";
	outbuffer += msg;
}

bool Client::updateAuthStatus()
{
	isAuthenticated = !nickname.empty() && !username.empty() && knewPassword;
	return isAuthenticated;
}
