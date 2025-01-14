#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleKICK(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() < 3)
		return ":irctic.com 461 KICK :Not enough parameters"; // ERR_NEEDMOREPARAMS
	Channel *channel = server.getChannel(parts[1]);
	if (!channel)
		return ":irctic.com 403 " + parts[1] + " :No such channel"; // ERR_NOSUCHCHANNEL
	if (!client.isOperatorIn(channel))
		return ":irctic.com 482 " + parts[1] + " :You're not channel operator"; // ERR_CHANOPRIVSNEEDED

	Client *clientToKick = server.getClientByName(parts[2]);
	if (!clientToKick)
		return ":irctic.com 401 " + parts[2] + " :No such nick/channel"; // ERR_NOSUCHNICK
	channel->kick(clientToKick->id, server);

	if (parts.size() < 4)
		clientToKick->sendMessage("You have been kicked from " + channel->name + ".");
	else
		clientToKick->sendMessage("You have been kicked from " + channel->name + " (" + parts[3] + ").");
	return ":irctic.com 200 KICK " + channel->name + " " + parts[2] + " :Kicked";
}
