#include "BotSocket.hpp"

void BotSocket::setupListener()
{
	_listener = socket(AF_INET, SOCK_STREAM, 0);
	if (_listener < 0)
		throw std::runtime_error("Socket creation failed");

	int opt = 1;
	if (setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Setsockopt failed");

	fcntl(_listener, F_SETFL, O_NONBLOCK);

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(42069);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Bind failed");

	if (listen(_listener, 42) < 0)
		throw std::runtime_error("Listen failed");

	struct pollfd pfd;
	pfd.fd = _listener;
	pfd.events = POLLIN;
	_pollFds.push_back(pfd);

	std::cout << "🤘(ﾉ^_^)ﾉ IRC Server listening on port 42069 with a stylish backlog of 42!\n";
}
