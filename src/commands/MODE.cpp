#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleMODE(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() < 3 || parts.size() > 4)
		return ":irctic.com 461 MODE :Not enough parameters"; // ERR_NEEDMOREPARAMS
	Channel *channel = server.getChannel(parts[1]);
	if (!client.isOperatorIn(channel))
		return ":irctic.com 403 " + parts[1] + " :No such channel"; // ERR_NOSUCHCHANNEL
	if (!channel)
		return ":irctic.com 482 " + parts[1] + " :You're not channel operator"; // ERR_CHANOPRIVSNEEDED

	std::string mode = parts[2];
	if (mode == "+i")
	{
		channel->inviteOnly = true;
		channel->broadcast(":irctic.com MODE " + channel->name + " +i", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-i")
	{
		channel->inviteOnly = false;
		channel->broadcast(":irctic.com MODE " + channel->name + " -i", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+t")
	{
		channel->anyoneCanChangeTopic = true;
		channel->broadcast(":irctic.com MODE " + channel->name + " +t", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-t")
	{
		channel->anyoneCanChangeTopic = false;
		channel->broadcast(":irctic.com MODE " + channel->name + " -t", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+k")
	{
		if (parts.size() != 4)
			return ":irctic.com 461 MODE +k :Not enough parameters"; // ERR_NEEDMOREPARAMS
		channel->password = parts[3];
		channel->broadcast(":irctic.com MODE " + channel->name + " +k " + channel->password + "", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-k")
	{
		channel->password = "";
		channel->broadcast(":irctic.com MODE " + channel->name + " -k", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+l")
	{
		if (parts.size() != 4)
			return ":irctic.com 461 MODE +l :Not enough parameters"; // ERR_NEEDMOREPARAMS
		channel->limit = std::stoi(parts[3]);
		channel->broadcast(":irctic.com MODE " + channel->name + " +l " + std::to_string(channel->limit) + "", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-l")
	{
		channel->limit = 0;
		channel->broadcast(":irctic.com MODE " + channel->name + " -l", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+o")
	{
		if (parts.size() != 4)
			return ":irctic.com 461 MODE +o :Not enough parameters"; // ERR_NEEDMOREPARAMS
		unsigned int clientIdToOp = server.getClientIdByName(parts[3]);
		if (clientIdToOp > 0)
		{
			channel->addOperator(clientIdToOp);
			channel->broadcast(":irctic.com MODE " + channel->name + " +o " + parts[3] + "", server); // RPL_CHANNELMODEIS
		}
		else
			return ":irctic.com 401 " + parts[3] + " :No such nick/channel"; // ERR_NOSUCHNICK
	}
	else if (mode == "-o")
	{
		if (parts.size() != 4)
			return ":irctic.com 461 MODE -o :Not enough parameters"; // ERR_NEEDMOREPARAMS
		unsigned int clientIdToDeop = server.getClientIdByName(parts[3]);
		if (clientIdToDeop > 0)
		{
			channel->removeOperator(clientIdToDeop);
			channel->broadcast(":irctic.com MODE " + channel->name + " -o " + parts[3] + "", server); // RPL_CHANNELMODEIS
		}
		else
			return ":irctic.com 401 " + parts[3] + " :No such nick/channel"; // ERR_NOSUCHNICK
	}
	else
		return ":irctic.com 501 " + mode + " :Unknown MODE flag"; // ERR_UMODEUNKNOWNFLAG
	
	return "";
}
