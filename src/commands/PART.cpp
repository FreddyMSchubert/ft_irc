#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandlePART(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return ":irctic.com 451 PART :You have not registered"; // ERR_NOTREGISTERED
	if (!client.channel)
		return ":irctic.com 442 * :You're not on any channel"; // ERR_NOTONCHANNEL
	if (parts.size() < 2)
		return ":irctic.com 461 PART :Not enough parameters"; // ERR_NEEDMOREPARAMS

	std::string partMessage = "PART " + client.channel->name;
	if (parts.size() > 1)
		partMessage += " :" + parts[1];
	partMessage += "\r\n";

	client.channel->removeMember(client.id, server);
	client.channel = nullptr;

	return partMessage;
}
