#include "Bot.hpp"
#include <stdexcept>

Socket::Socket()
{
	_socket_ip = "";
	_socket_port = -1;
}

Socket::~Socket() { }

void Socket::setNonBlocking()
{
	int flags = fcntl(_socket_fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Failed to get socket " + std::to_string(_socket_fd) + " flags");
	if (fcntl(_socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode on socket " + std::to_string(_socket_fd));
}

void Socket::connectToServer(std::string ip, int port)
{
	if (ip.empty() || port <= 0)
		throw std::runtime_error("IP or port not set");

	_socket_ip = ip;
	_socket_port = port;

	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
		throw std::runtime_error("Socket creation failed");

	try
	{
		std::memset(&_socket, 0, sizeof(_socket));
		_socket.sin_family = AF_INET;
		_socket.sin_port = htons(this->_socket_port);

		if (inet_pton(AF_INET, this->_socket_ip.c_str(), &_socket.sin_addr) <= 0)
		{
			close(_socket_fd);
			throw std::runtime_error("Invalid IP address");
		}

		int opt = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		
		if (bind(_socket_fd, (struct sockaddr *)&_socket, sizeof(_socket)) < 0)
		{	
			close(_socket_fd);
			throw std::runtime_error("Failed to bind socket" + std::to_string(_socket_fd));
		}

		if (listen(_socket_fd, 10) == -1)
		{
			close(_socket_fd);
			throw std::runtime_error("Failed to listen on socket");
		}

		setNonBlocking();
	}
	catch(const std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
	executeEventsOfType(EventType::ON_CONNECT);
}

void Socket::addEventListener(EventType type, EventCallback callback)
{
	_events.push_back({type, callback});
}

void Socket::sendMessage(std::string msg)
{
	if (send(_socket_fd, msg.c_str(), msg.length(), 0) == -1)
	{
		executeEventsOfType(EventType::ON_ERROR);
	}
}

void Socket::executeEventsOfType(EventType type, std::string msg)
{
	for (auto &event : _events)
	{
		if (event.type == type)
			event.callback(msg);
	}
}

void Socket::Run()
{
	// Poll for events
	struct pollfd fds[1];
	fds[0].fd = _socket_fd;
	fds[0].events = POLLIN;

	while (true)
	{
		int ret = poll(fds, 1, 1000);
		if (ret == -1)
			executeEventsOfType(EventType::ON_ERROR);
		else if (ret > 0)
		{
			if (fds[0].revents & POLLIN)
			{
				char buffer[1024] = {0};
				int valread = read(_socket_fd, buffer, 1024);
				if (valread == 0)
				{
					executeEventsOfType(EventType::ON_DICONNECT);
					break;
				}
				else
				{
					executeEventsOfType(EventType::ON_MESSAGE, std::string(buffer));
				}
			}
		}
	}
}