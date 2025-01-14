#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleNICK(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 2)
		return ":irctic.com 461 NICK :Not enough parameters"; // ERR_NEEDMOREPARAMS
	if (parts[1][0] == '#')
		return ":irctic.com 432 * " + parts[1] + " :Erroneous nickname"; // ERR_ERRONEUSNICKNAME

	for (auto &c : server.getClients())
		if (c.nickname == parts[1])
			return ":irctic.com 433 * " + parts[1] + " :Nickname is already in use"; // ERR_NICKNAMEINUSE
	client.nickname = parts[1];
	if (client.updateAuthStatus())
			CompleteHandshake(client);
	return ":irctic.com NICK " + client.nickname; // NICK change broadcast
}
