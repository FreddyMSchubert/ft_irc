#include "../inc/ConnectionManager.hpp"
#include "../inc/Socket.hpp"

//Setup the listening socket and push it to the vector of sockets
ConnectionManager::ConnectionManager(int port) : _port(port), _listening_socket{port}
{
	Logger::Log(LogLevel::INFO, "Server initialized on port " + std::to_string(port));
}

// POLLIN    // Ready to read (incoming requests/data)
// POLLOUT   // Ready to write (sending responses)
// POLLHUP   // Client disconnected
// POLLERR   // Error occurred
void ConnectionManager::updatePoll()
{
	// Create pollfds for poll call
	std::vector<struct pollfd> fds;
	for (size_t i = 0; i < _sockets.size(); ++i)
	{
		struct pollfd pfd;
		pfd.fd = _sockets[i].socket.getSocketFd();
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
		_sockets[i].states.read = fds[i].revents & POLLIN;
		_sockets[i].states.write = fds[i].revents & POLLOUT;
		_sockets[i].states.disconnect = fds[i].revents & POLLHUP;
		_sockets[i].states.error = fds[i].revents & POLLERR;
	}
}

void ConnectionManager::acceptNewConnections()
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

void ConnectionManager::handleExistingConnections()
{
	for (int i = (int)_sockets.size() - 1; i >= 0; i--)
	{
		if (_sockets[i].states.disconnect || _sockets[i].states.error)
		{
			if (_sockets[i].states.disconnect)
				Logger::Log(LogLevel::INFO, "Client disconnected");
			else
				Logger::Log(LogLevel::ERROR, "Error occurred on client socket: " + std::string(strerror(errno)));
			_sockets.erase(_sockets.begin() + i);
			continue;
		}
		if (_sockets[i].states.read)
		{
			Logger::Log(LogLevel::INFO, "Reading data from client");
			try
			{
				_sockets[i].buffer += _sockets[i].socket.receiveData();
			}
			catch(const std::runtime_error &e)
			{
				Logger::Log(LogLevel::WARNING, "Erroneous message data: " + std::string(e.what()));
				_sockets.erase(_sockets.begin() + i);
				continue;
			}
		}
		if (_sockets[i].states.write)
		{
			// send somethin back

			// Logger::Log(LogLevel::INFO, "Determining response to client");
			// Request req(_sockets[i].buffer.str());
			// Response res(req, _config);

			// std::vector<t_location> locations = get_locations(_config, req.getPath());
			// for (const auto & loc : locations)
			// 	if (!loc.redirections.empty())
			// 		_sockets[i].socket.redirectToOtherResource(loc.redirections[0].second, loc.redirections[0].first);

			// Logger::Log(LogLevel::INFO, _config.getServerId(), "Sending response to client");
			// try
			// {
			// 	std::cout << (res.getStatus() == Status::OK) << std::endl;
			// 	if (res.getStatus() != Status::OK)
			// 		_sockets[i].socket.redirectToError(res.getStatus());
			// 	else
			// 		_sockets[i].socket.sendData(res.getRawPacket());
			// }
			// catch (const std::exception &e)
			// {
			// 	Logger::Log(LogLevel::ERROR, _config.getServerId(), "Failed to send response: " + std::string(e.what()));
			// }
			// Logger::Log(LogLevel::INFO, _config.getServerId(), "Response sent to client");
			// _sockets.erase(_sockets.begin() + i);
			// continue;
		}
	}
}

void ConnectionManager::Run()
{
	while (42 == 42) // to exit program, please send sigint
	{
		updatePoll();
		acceptNewConnections();
		handleExistingConnections();
	}
}
