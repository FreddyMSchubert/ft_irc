#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandlePASS(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 2)
		return ":irctic.com 461 PASS :Not enough parameters"; // ERR_NEEDMOREPARAMS
	if (client.knewPassword)
		return ":irctic.com 462 * :You already inputted the password correctly."; // ERR_ALREADYREGISTRED

	if (server.isCorrectPassword(parts[1]))
	{
		client.knewPassword = true;
		if (client.updateAuthStatus())
			CompleteHandshake(client);
		return ":irctic.com 900 * :Password correct"; // Custom success reply
	}
	return ":irctic.com 464 * :Password incorrect"; // ERR_PASSWDMISMATCH
}
