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

		if (connect(_socket_fd, (struct sockaddr *)&_socket, sizeof(_socket)) < 0)
		{
			if (errno != EINPROGRESS)
			{
				close(_socket_fd);
				executeEventsOfType(EventType::ON_ERROR, std::string(strerror(errno)));
				throw std::runtime_error("Failed to connect to server: " + std::string(strerror(errno)));
			}
		}
		else
		{
			std::cout << "Connected to server: " << ip << ":" << port << std::endl;
		}
	}
	catch(const std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
	executeEventsOfType(EventType::ON_CONNECT, "Connected to server");
}

void Socket::addEventListener(EventType type, EventCallback callback)
{
	_events.push_back({type, callback});
}

void Socket::queueMessage(std::string msg)
{
	_messages.emplace_back(msg);
}

void Socket::sendMessage(std::string msg)
{
	ssize_t sent = send(_socket_fd, msg.c_str(), msg.length(), 0);
	if (sent == -1)
	{
		if (errno == EPIPE)
		{
			std::cerr << "Error: Broken pipe (EPIPE) - Disconnected from server" << std::endl;
			executeEventsOfType(EventType::ON_DISCONNECT, "Disconnected from server");
			running = false;
		}
		else
		{
			std::cerr << "Error: Failed to send message: " << strerror(errno) << std::endl;
			executeEventsOfType(EventType::ON_ERROR, "Failed to send message");
		}
	}
	else if (sent < msg.length())
	{
		std::cerr << "Warning: Partial message sent: " << sent << " of " << msg.length() << " bytes" << std::endl;
	}
	else
	{
		std::cout << "Message sent successfully: " << msg << std::endl;
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
	fds[0].events = POLLIN | POLLOUT;

	setNonBlocking();

	while (running)
	{
		int ret = poll(fds, 1, 1000);
		if (ret == -1)
		{
			executeEventsOfType(EventType::ON_ERROR, "Poll error: " + std::string(strerror(errno)));
		}
		else if (ret == 0)
		{
			std::cout << "Poll timeout, no events" << std::endl;
		}
		else if (ret > 0)
		{
			if (fds[0].revents & POLLIN)
			{
				std::cout << "POLLIN event detected" << std::endl;
				char buffer[1024] = {0};
				std::memset(buffer, 0, sizeof(buffer));
				int valread = read(_socket_fd, buffer, sizeof(buffer) - 1);
				if (valread == 0)
				{
					std::cerr << "Server closed the connection" << std::endl;
					executeEventsOfType(EventType::ON_DISCONNECT, "Disconnected from server");
					break;
				}
				else if (valread < 0)
				{
					std::cerr << "Read error: " << strerror(errno) << std::endl;
					executeEventsOfType(EventType::ON_ERROR, "Read error: " + std::string(strerror(errno)));
				}
				else
				{
					std::cout << "Data read from server: " << buffer << std::endl;
					executeEventsOfType(EventType::ON_MESSAGE, std::string(buffer));
				}
			}
			if (fds[0].revents & POLLOUT)
			{
				std::cout << "POLLOUT event detected" << std::endl;
				if (!_messages.empty())
				{
					sendMessage(_messages.front());
					_messages.erase(_messages.begin());
				}
			}
			if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				std::cerr << "Poll error event detected: " << fds[0].revents << std::endl;
				executeEventsOfType(EventType::ON_ERROR, "Poll error event detected");
				break;
			}
		}
	}
}