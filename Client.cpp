#include "Client.hpp"
#include <sstream>

Client::Client() : _fd(-1), _host(""), _nickname(""), _username(""), _isAuthenticated(false) {}

Client::Client(int fd, const std::string &host)
: _fd(fd), _host(host), _nickname(""), _username(""), _isAuthenticated(false)
{
}

void Client::appendBuffer(const std::string &data)
{
	_buffer += data;
}

bool Client::extractLine(std::string &line)
{
	size_t pos = _buffer.find("\n");
	if (pos == std::string::npos)
		return false;

	line = _buffer.substr(0, pos);
	if (!line.empty() && line[line.size()-1] == '\r')
		line.erase(line.size()-1);

	_buffer.erase(0, pos + 1);
	return true;
}
