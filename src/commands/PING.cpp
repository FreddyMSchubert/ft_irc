#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandlePING(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)client;
	(void)server;
	if (parts.size() < 2)
		return ":irctic.com PONG irctic.com";
	return ":irctic.com PONG irctic.com :" + parts[1];
}
