#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleJOIN(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return ":irctic.com 451 JOIN :You have not registered"; // ERR_NOTREGISTERED
	if (parts.size() < 2 || parts.size() > 3)
	{
		std::string response = ":irctic.com 461 JOIN :Not enough parameters"; // ERR_NEEDMOREPARAMS
		if (client.channel)
			response += ":irctic.com 442 " + client.channel->name + " :You are already on channel"; // ERR_USERONCHANNEL
		if (server.getChannels().size() > 0)
		{
			response += ":irctic.com 353 " + client.nickname + " = :"; // RPL_NAMREPLY
			for (auto &channel : server.getChannels())
				response += channel.name + " ";
			response += "\r\n";
			response += ":irctic.com 366 " + client.nickname + " :End of NAMES list"; // RPL_ENDOFNAMES
		}
		return response;
	}

	std::string channelName = parts[1];
	Channel *channel = server.getChannel(channelName);
	if (!channel)
	{
		if (channelName[0] != '#')
			return ":irctic.com 476 " + channelName + " :Invalid channel name"; // ERR_BADCHANMASK
		server.createChannel(channelName);
		channel = server.getChannel(channelName);
	}

	if (channel->password != "" && (parts.size() < 3 || parts[2] != channel->password))
		return ":irctic.com 475 " + channelName + " :Cannot join channel (+k)\r\n"; // ERR_BADCHANNELKEY

	std::string channelJoinReturn = channel->addMember(client.id, server);
	client.channel = channel;

	return channelJoinReturn;
}
