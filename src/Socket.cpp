#include "../inc/Socket.hpp"

// Listening socket constructor
Socket::Socket(std::string server_ip, int server_port) : m_server_ip(server_ip), m_server_port(server_port)
{
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd == -1)
		throw std::runtime_error("Socket creation failed");

	try
	{
		std::memset(&m_socket, 0, sizeof(m_socket));
		m_socket.sin_family = AF_INET;
		m_socket.sin_port = htons(m_server_port);

		if (inet_pton(AF_INET, m_server_ip.c_str(), &m_socket.sin_addr) <= 0)
		{
			close(m_socket_fd);
			throw std::runtime_error("Invalid IP address");
		}

		int opt = 1;
		if (setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		
		if (bind(m_socket_fd, (struct sockaddr *)&m_socket, sizeof(m_socket)) < 0)
		{	
			close(m_socket_fd);
			throw std::runtime_error("Failed to bind socket" + std::to_string(m_socket_fd));
		}

		if (listen(m_socket_fd, 10) == -1)
		{
			close(m_socket_fd);
			throw std::runtime_error("Failed to listen on socket");
		}

		Logger::Info("Listening on " + m_server_ip + ":" + std::to_string(m_server_port));
		setNonBlocking();
	}
	catch(const std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
}

// Client socket constructor
Socket::Socket(int fd) : m_socket_fd(fd)
{
	setNonBlocking();
	Logger::Info("Running Client Connection Socket " + std::to_string(m_socket_fd) + "...");
}

Socket::Socket(Socket&& other) noexcept : m_socket_fd(other.m_socket_fd), m_socket(other.m_socket)
{
	other.m_socket_fd = -1; // Mark as moved; prevents double close
}
Socket& Socket::operator=(Socket&& other) noexcept
{
	if (this == &other)
		return *this;
	if (m_socket_fd >= 0)
		close(m_socket_fd);
	m_socket_fd = other.m_socket_fd;
	m_socket = other.m_socket;
	other.m_socket_fd = -1; // Mark as moved; prevents double close
	return *this;
}
Socket::~Socket()
{
	if (m_socket_fd >= 0)
	{
		close(m_socket_fd);
		Logger::Info("Socket " + std::to_string(m_socket_fd) + " closed");
	}
}

void Socket::sendData(std::string data)
{
	ssize_t sent = send(m_socket_fd, data.c_str(), data.length(), 0);
	if (sent < 0)
		throw std::runtime_error("Failed to send data");
	else if (sent < data.length())
		Logger::Error("Failed to send all data");
	else
		Logger::Info("Data sent successfully");
}

std::string Socket::receiveData()
{
	std::string data;
	int size = 1000;
	char buffer[size];
	ssize_t received;

	received = recv(m_socket_fd, buffer, sizeof(buffer), 0);
	if (received > 0)
		data.append(buffer, received);
	else if (received <= 0)
	{
		Logger::Warning("Failed to receive data");
		return "";
	}

	return data;
}

void Socket::setNonBlocking()
{
	int flags = fcntl(m_socket_fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Failed to get socket " + std::to_string(m_socket_fd) + " flags");
	if (fcntl(m_socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode on socket " + std::to_string(m_socket_fd));
}