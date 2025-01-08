#include "../inc/Server.hpp"
#include "../inc/Socket.hpp"
#include <sys/socket.h>

Server::Server(std::string ip, int port, std::string password) : m_listening_socket(-1, Socket(ip, port)),   m_ip(ip), m_port(port), m_password(password)
{
	Logger::Info("Running Server on " + m_ip + ":" + std::to_string(m_port) + "...");
}
// POLLIN    // Ready to read (incoming requests/data)
// POLLOUT   // Ready to write (sending responses)
// POLLHUP   // Client disconnected
// POLLERR   // Error occurred
void Server::updatePoll()
{
	// Create pollfds for poll calls
	std::vector<struct pollfd> fds;
	for (size_t i = 0; i < m_sockets.size(); ++i)
	{
		struct pollfd pfd;
		pfd.fd = m_sockets[i].socket.getSocketFd();
		pfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
		fds.push_back(pfd);
	}
	struct pollfd listen_pfd;
	listen_pfd.fd = m_listening_socket.socket.getSocketFd();
	listen_pfd.events = POLLIN | POLLOUT | POLLERR | POLLHUP;
	fds.push_back(listen_pfd);

	// poll
	int ret = poll(fds.data(), fds.size(), 0);
	if (ret < 0)
	{
		Logger::Error("Poll error: " + std::string(strerror(errno)) + " -> means that there is no data to read.");
		return;
	}

	if (ret == 0) return;

	// Read out poll data
	m_listening_socket.states.read = fds[fds.size() - 1].revents & POLLIN;
	m_listening_socket.states.write = fds[fds.size() - 1].revents & POLLOUT;
	m_listening_socket.states.disconnect = fds[fds.size() - 1].revents & POLLHUP;
	m_listening_socket.states.error = fds[fds.size() - 1].revents & POLLERR;
	for (size_t i = 0; i < fds.size() - 1; i++)
	{
		m_sockets[i].states.read = fds[i].revents & POLLIN;
		m_sockets[i].states.write = fds[i].revents & POLLOUT;
		m_sockets[i].states.disconnect = fds[i].revents & POLLHUP;
		m_sockets[i].states.error = fds[i].revents & POLLERR;
	}
}

void Server::acceptNewConnections()
{
	if (!m_listening_socket.states.read)
		return;
	int max_iterations = 10;
	while (max_iterations > 0)
	{
		struct sockaddr_in client_addr;
		socklen_t addrlen = sizeof(struct sockaddr_in);
		int client_fd = accept(m_listening_socket.socket.getSocketFd(), (struct sockaddr*)&client_addr, &addrlen);
		if (client_fd >= 0)
		{
			Logger::Info("New client connected");
			m_sockets.emplace_back(client_fd, Socket(client_fd));
			max_iterations--;
		}
		else
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK) // i believe accept is not an i/o operation
				Logger::Info("Accept error: " + std::string(strerror(errno)));
			break;
		}
	}
}

void Server::handleExistingConnections()
{
	for (int i = (int)m_sockets.size() - 1; i >= 0; i--)
	{
		if (m_sockets[i].states.read)
		{
			Logger::Info("Reading data from client");
			try
			{
				m_sockets[i].buffer << m_sockets[i].socket.receiveData();
				if (m_sockets[i].buffer.str().empty()) continue;
			}
			catch(const std::runtime_error &e)
			{
				Logger::Info("Erroneous packet data: " + std::string(e.what()));
				// m_sockets.erase(m_sockets.begin() + i);
				continue;
			}
		}
		if (m_sockets[i].states.write)
		{
			Logger::Info("Sending response to client");
			m_sockets[i].socket.sendData("Test response");
			// m_sockets.erase(m_sockets.begin() + i);

			continue;
		}
		if (m_sockets[i].states.disconnect || m_sockets[i].states.error)
		{
			if (m_sockets[i].states.disconnect)
				Logger::Info("Client disconnected");
			else
				Logger::Info("Error occurred on client socket: " + std::string(strerror(errno)));
			m_sockets.erase(m_sockets.begin() + i);
		}
	}
}

void Server::Run()
{
	while (1)
	{
		updatePoll();
		acceptNewConnections();
		handleExistingConnections();
	}
}