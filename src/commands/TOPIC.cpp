#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleTOPIC(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 3)
	{
		if (client.channel && client.channel->topic != "")
			return ":irctic.com 331 " + client.channel->name + " :" + client.channel->topic + ""; // RPL_NOTOPIC
		return ":irctic.com 461 TOPIC :Not enough parameters"; // ERR_NEEDMOREPARAMS
	}

	Channel *channel = server.getChannel(parts[1]);
	if (!channel)
		return ":irctic.com 403 " + parts[1] + " :No such channel"; // ERR_NOSUCHCHANNEL
	if (!client.isOperatorIn(channel) && !channel->anyoneCanChangeTopic)
		return ":irctic.com 482 " + channel->name + " :You're not channel operator"; // ERR_CHANOPRIVSNEEDED
	
	channel->topic = parts[2];
	channel->broadcast(":irctic.com 332 " + channel->name + " :" + channel->topic, server); // RPL_TOPIC
	return ":irctic.com 332 " + channel->name + " :" + channel->topic; // Confirmation
}
