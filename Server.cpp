#include "Server.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Server::Server(int port, const std::string &password)
: _port(port), _password(password), _listener(-1), _running(true)
{
	setupListener();
}

Server::~Server()
{
	if (_listener >= 0)
		close(_listener);
}

void Server::setupListener()
{
	_listener = socket(AF_INET, SOCK_STREAM, 0);
	if (_listener < 0)
		throw std::runtime_error("Socket creation failed");

	// Make socket reusable
	int opt = 1;
	if (setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Setsockopt failed");

	// Non-blocking
	fcntl(_listener, F_SETFL, O_NONBLOCK);

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Bind failed");

	if (listen(_listener, 42) < 0) // 42 - magical backlog queue (metric system?).
		throw std::runtime_error("Listen failed");

	struct pollfd pfd;
	pfd.fd = _listener;
	pfd.events = POLLIN;
	_pollFds.push_back(pfd);

	std::cout << "🤘(ﾉ^_^)ﾉ IRC Server listening on port " << _port 
			  << " with a stylish backlog of 42!\n";
}

void Server::run()
{
	while (_running)
	{
		int ready = poll(&_pollFds[0], _pollFds.size(), 5000); // 5 seconds poll
		if (ready < 0)
		{
			if (errno == EINTR) // Interruption by signal
				continue;
			throw std::runtime_error("Poll error");
		}
		if (ready == 0)
		{
			// Possibly do housekeeping or keep-alive checks
			continue;
		}

		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _listener)
				{
					acceptNewClient();
				}
				else
				{
					handleClientData(_pollFds[i].fd);
				}
			}
			// If needed, handle POLLOUT for queued data, etc.
		}
	}
}

void Server::acceptNewClient()
{
	sockaddr_in clientAddr;
	socklen_t addrSize = sizeof(clientAddr);
	int clientFd = accept(_listener, (struct sockaddr *)&clientAddr, &addrSize);
	if (clientFd < 0)
		return;

	// Non-blocking
	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	// Add to poll
	struct pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	_pollFds.push_back(pfd);

	// Create Client instance
	_clients[clientFd] = Client(clientFd, inet_ntoa(clientAddr.sin_addr));
	std::cout << "✋ (☞ﾟヮﾟ)☞ New client accepted: FD = " << clientFd << "\n";
}

void Server::handleClientData(int fd)
{
	char buf[512];
	std::memset(buf, 0, sizeof(buf));
	ssize_t bytes = recv(fd, buf, 511, 0);
	if (bytes <= 0)
	{
		// Client disconnected or error
		disconnectClient(fd);
		return;
	}

	// Accumulate data, commands may come in partial
	Client &client = _clients[fd];
	client.appendBuffer(std::string(buf, bytes));

	// Try to parse commands
	std::string line;
	while (client.extractLine(line))
	{
		// Let the CommandHandler handle the line
		_cmdHandler.handleCommand(*this, fd, line);
	}
}

void Server::disconnectClient(int fd)
{
	std::cout << "💔 (╥_╥) Client disconnected: FD = " << fd << "\n";

	// Remove from poll vector
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}

	// Remove from server's client map
	_clients.erase(fd);

	close(fd);
}

bool Server::authenticateClient(const std::string &passAttempt)
{
	return (passAttempt == _password);
}

void Server::sendMessage(int fd, const std::string &msg)
{
	send(fd, msg.c_str(), msg.size(), 0);
}

void Server::broadcastChannel(const std::string &channelName, const std::string &msg, int excludeFd)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return;
	const std::map<int, bool> &members = chan->getMembers();
	for (std::map<int, bool>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (it->first != excludeFd)
			sendMessage(it->first, msg + "\r\n");
	}
}

Channel* Server::getChannel(const std::string &chanName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(chanName);
	if (it == _channels.end())
		return NULL;
	return &it->second;
}

Channel& Server::createChannel(const std::string &chanName, int creatorFd)
{
	_channels.insert(std::make_pair(chanName, Channel(chanName)));
	_channels[chanName].addMember(creatorFd, true); // add as operator
	return _channels[chanName];
}

void Server::removeEmptyChannel(const std::string &chanName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(chanName);
	if (it != _channels.end())
	{
		if (it->second.getMembers().empty())
			_channels.erase(it);
	}
}

Client* Server::getClient(int fd)
{
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return NULL;
	return &it->second;
}

std::map<int, Client> &Server::getAllClients()
{
	return _clients;
}

Bot& Server::getBot()
{
	return _bot;
}

FileTransfer& Server::getFileTransfer()
{
	return _fileTransfer;
}