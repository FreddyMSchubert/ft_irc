#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleUSER(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() < 2)
		return ":irctic.com 461 USER :Not enough parameters"; // ERR_NEEDMOREPARAMS
	if (parts[1].size() <= 0 || parts[1][0] == '#')
		return ":irctic.com 432 * " + parts[1] + " :Erroneous username"; // ERR_ERRONEUSNICKNAME

	for (auto &c : server.getClients())
		if (c.username == parts[1])
			return ":irctic.com 464 * :Username is already in use"; // Custom error for duplicate username
	client.username = parts[1];
	if (client.updateAuthStatus())
			CompleteHandshake(client);
	return "Your username is now \"" + parts[1] + "\".";
}
