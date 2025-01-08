#include "../inc/Server.hpp"
#include "../inc/Socket.hpp"

//Setup the listening socket and push it to the vector of sockets
Server::Server(int port, std::string password, std::string op_password) : _port(port), _listening_socket(port), _password(password), _op_password(op_password)
{
	Logger::Log(LogLevel::INFO, "Server initialized on port " + std::to_string(port));
}

void Server::Run()
{
	while (42 == 42) // to exit program, please send sigint
	{
		updatePoll();
		acceptNewConnections();
		handleExistingConnections();
	}
}

/* ----------------------- */
/* ----- CONNECTIONS ----- */
/* ----------------------- */

// POLLIN    // Ready to read (incoming requests/data)
// POLLOUT   // Ready to write (sending responses)
// POLLHUP   // Client disconnected
// POLLERR   // Error occurred
void Server::updatePoll()
{
	// Create pollfds for poll call
	std::vector<struct pollfd> fds;
	for (size_t i = 0; i < _sockets.size(); ++i)
	{
		struct pollfd pfd;
		pfd.fd = _sockets[i]->socket.getSocketFd();
		pfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
		fds.push_back(pfd);
	}
	struct pollfd listen_pfd;
	listen_pfd.fd = _listening_socket.socket.getSocketFd();
	listen_pfd.events = POLLIN | POLLOUT | POLLERR | POLLHUP;
	fds.push_back(listen_pfd);

	// poll
	int ret = poll(fds.data(), fds.size(), 50);
	if (ret < 0)
	{
		Logger::Log(LogLevel::ERROR, "Poll error: " + std::string(strerror(errno)) + " -> means that there is no data to read.");
		return;
	}

	// Read out poll data
	_listening_socket.states.read = fds[fds.size() - 1].revents & POLLIN;
	_listening_socket.states.write = fds[fds.size() - 1].revents & POLLOUT;
	_listening_socket.states.disconnect = fds[fds.size() - 1].revents & POLLHUP;
	_listening_socket.states.error = fds[fds.size() - 1].revents & POLLERR;
	for (size_t i = 0; i < fds.size() - 1; i++)
	{
		_sockets[i]->states.read = fds[i].revents & POLLIN;
		_sockets[i]->states.write = fds[i].revents & POLLOUT;
		_sockets[i]->states.disconnect = fds[i].revents & POLLHUP;
		_sockets[i]->states.error = fds[i].revents & POLLERR;
	}
}

void Server::acceptNewConnections()
{
	if (!_listening_socket.states.read)
		return;
	int max_iterations = 10;
	while (max_iterations > 0)
	{
		struct sockaddr_in client_addr;
		socklen_t addrlen = sizeof(struct sockaddr_in);
		int client_fd = accept(_listening_socket.socket.getSocketFd(), (struct sockaddr*)&client_addr, &addrlen);
		if (client_fd >= 0)
		{
			Logger::Log(LogLevel::INFO, "New client connected");
			_sockets.emplace_back(client_fd, Socket(client_fd, _port));
			max_iterations--;
		}
		else
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK) // i believe accept is not an i/o operation
				Logger::Log(LogLevel::ERROR, "Accept error: " + std::string(strerror(errno)));
			break;
		}
	}
}

void Server::handleExistingConnections()
{
	for (int i = (int)_sockets.size() - 1; i >= 0; i--)
	{
		if (_sockets[i]->states.disconnect || _sockets[i]->states.error)
		{
			if (_sockets[i]->states.disconnect)
				Logger::Log(LogLevel::INFO, "Client disconnected");
			else
				Logger::Log(LogLevel::ERROR, "Error occurred on client socket: " + std::string(strerror(errno)));
			_sockets.erase(_sockets.begin() + i);
			continue;
		}
		if (_sockets[i]->states.read)
		{
			Logger::Log(LogLevel::INFO, "Reading data from client");
			try
			{
				_sockets[i]->inbuffer += _sockets[i]->socket.receiveData();
			}
			catch(const std::runtime_error &e)
			{
				Logger::Log(LogLevel::WARNING, "Erroneous message data: " + std::string(e.what()));
				_sockets.erase(_sockets.begin() + i);
				continue;
			}
			HandleClientData(_sockets[i]);
		}
		if (_sockets[i]->states.write && !_sockets[i]->outbuffer.empty())
		{
			Logger::Log(LogLevel::INFO, "Sending response to client");
			try
			{
				_sockets[i]->socket.sendData(_sockets[i]->outbuffer);
				_sockets[i]->outbuffer.clear();
			}
			catch (const std::exception &e)
			{
				Logger::Log(LogLevel::ERROR, "Failed to send response: " + std::string(e.what()));
			}
			Logger::Log(LogLevel::INFO, "Response sent to client");
			continue;
		}
	}
}

/* ----------------------- */
/* ------ IRC LOGIC ------ */
/* ----------------------- */

void Server::HandleClientData(std::shared_ptr<Client> & client)
{
	// 1. get a line from the inbuffer, call commandhandler to generate a line for outbuffer
	size_t newLinePos = client->inbuffer.find("\n");
	if (newLinePos == std::string::npos)
		return;
	std::string line = client->inbuffer.substr(0, newLinePos);
	client->inbuffer.erase(0, newLinePos + 1);
	std::string response = CommandHandler::HandleCommand(line, client, *this);
	client->outbuffer += response + "\n";
}

bool Server::isCorrectPassword(std::string passwordAttempt)
{
	return passwordAttempt == _password;
}
bool Server::isCorrectOperatorPassword(std::string passwordAttempt)
{
	return passwordAttempt == _op_password;
}

Channel *Server::getChannel(std::string channelName)
{
	for (auto& channel_ptr : _channels)
		if (channel_ptr.name == channelName)
			return &channel_ptr;
	return nullptr;
}
void Server::createChannel(std::string channelName)
{
	_channels.emplace_back(channelName);
	Logger::Log(LogLevel::INFO, "Created new channel: " + channelName);
}
