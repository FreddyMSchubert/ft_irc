#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
private:
	int         _fd;
	std::string _host;
	std::string _nickname;
	std::string _username;
	bool        _isAuthenticated;
	std::string _buffer;

public:
	Client();
	Client(int fd, const std::string &host);

	void setNickname(const std::string &nick)   { _nickname = nick; }
	void setUsername(const std::string &user)   { _username = user; }
	void setAuthenticated(bool auth)            { _isAuthenticated = auth; } // password is just to join. nothing without this

	int  getFd() const                          { return _fd; }
	std::string getNickname() const             { return _nickname; }
	std::string getUsername() const             { return _username; }
	bool isAuthenticated() const                { return _isAuthenticated; }

	void appendBuffer(const std::string &data);
	bool extractLine(std::string &line);
};

#endif
