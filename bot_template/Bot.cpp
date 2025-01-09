#include "Bot.hpp"



Bot::Bot(std::string ip, int port, std::string password)
{
	if (!ip.empty())
		_ip = ip;
	if (port > 0)
		_port = port;
	if (!password.empty())
		_password = password;
}

Bot::~Bot()
{
}
