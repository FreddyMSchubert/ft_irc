#include "../inc/CommandHandler.hpp"

#include "../inc/Server.hpp" // avoiding circular dependency

static std::vector<std::string> split(const std::string &str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim))
		tokens.push_back(token);
	return tokens;
}

std::string CommandHandler::CompleteHandshake(unsigned int clientId, Server & server)
{
	Client *clientPtr = server.getClientById(clientId);
	if (!clientPtr)
		return "Client not found";
	Client &client = *clientPtr;

	if (client.isAuthenticated && !client.hasReceivedWelcome)
	{
		std::string welcomeMsg = ":irctic.com 001 " + client.nickname + " :Welcome to the IRCtic, " + client.nickname + "!";
		client.sendMessage(welcomeMsg);
		client.hasReceivedWelcome = true;
		Logger::Log(LogLevel::INFO, "Sent RPL_WELCOME to " + client.nickname);
		return welcomeMsg;
	}

	return "";
}

std::string CommandHandler::HandleCommand(std::string inCommand, unsigned int clientId, Server & server)
{
	Client *clientPtr = server.getClientById(clientId);
	if (!clientPtr)
		return ":irctic.com 401 * :No such nick/channel"; // ERR_NOSUCHNICK
	Client &client = *clientPtr;

	Logger::Log(LogLevel::INFO, "Received command from " + client.getName() + ": " + inCommand);

	std::vector<std::string> parts = split(inCommand, ' ');
	int partsSize = parts.size();
	if (parts.empty())
		return ":irctic.com 421 :Unknown command"; // ERR_UNKNOWNCOMMAND



	if (parts[0] == "PASS") // AUTHENTICATE
	{
		if (partsSize != 2)
			return ":irctic.com 461 PASS :Not enough parameters"; // ERR_NEEDMOREPARAMS
		if (client.knewPassword)
			return ":irctic.com 462 * :You already inputted the password correctly."; // ERR_ALREADYREGISTRED

		if (server.isCorrectPassword(parts[1]))
		{
			client.knewPassword = true;
			if (client.updateAuthStatus())
				CompleteHandshake(clientId, server);
			return ":irctic.com 900 * :Password correct"; // Custom success reply
		}
		return ":irctic.com 464 * :Password incorrect"; // ERR_PASSWDMISMATCH
	}



	else if (parts[0] == "CAP") // CAPABILITY NEGOTIATION
	{
		if (parts.size() < 2)
			return ":irctic.com 461 CAP :Not enough parameters"; // ERR_NEEDMOREPARAMS

		if (parts[1] == "LS")
			return ":irctic.com CAP * LS :"; // RPL_CAPLS
		else if (parts[1] == "REQ")
			return ":irctic.com CAP * ACK :"; // RPL_CAPACK
		else if (parts[1] == "END")
			return ":irctic.com CAP * END :"; // RPL_CAPEND
		return ":irctic.com 502 CAP :CAPABILLITIES command failed"; // ERR_NOPROTOOPT
	}



	else if (parts[0] == "PING") // are you there?
	{
		if (partsSize < 2)
			return ":irctic.com PONG irctic.com"; // Standard PONG response
		return ":irctic.com PONG irctic.com :" + parts[1]; // PONG with payload
	}



	else if (parts[0] == "OPER") // AUTHENTICATE AS OPERATOR
	{
		if (partsSize != 3)
			return ":irctic.com 461 OPER :Not enough parameters"; // ERR_NEEDMOREPARAMS
		if (!client.isAuthenticated)
			return ":irctic.com 451 * :You have not registered"; // ERR_NOTREGISTERED

		Client *clientPtr = server.getClientByName(parts[1]);
		if (server.isCorrectOperatorPassword(parts[2]))
		{
			clientPtr->isOperator = true;
			return ":irctic.com 381 * :You are now an IRC operator"; // RPL_YOUREOPER
		}
		std::cout << "Typed operator password: \"" << parts[2] << "\"" << std::endl;
		return ":irctic.com 464 * :Operator password incorrect"; // ERR_PASSWDMISMATCH
	}



	else if (parts[0] == "NICK") // SET NICKNAME
	{
		if (partsSize != 2)
			return ":irctic.com 461 NICK :Not enough parameters"; // ERR_NEEDMOREPARAMS
		if (parts[1][0] == '#')
			return ":irctic.com 432 * " + parts[1] + " :Erroneous nickname"; // ERR_ERRONEUSNICKNAME

		for (auto &c : server.getClients())
			if (c.nickname == parts[1])
				return ":irctic.com 433 * " + parts[1] + " :Nickname is already in use"; // ERR_NICKNAMEINUSE
		client.nickname = parts[1];
		if (client.updateAuthStatus())
				CompleteHandshake(clientId, server);
		return ":irctic.com NICK " + client.nickname; // NICK change broadcast
	}



	else if (parts[0] == "USER" || parts[0] == "userhost") // SET USERNAME
	{
		if (partsSize < 2)
			return ":irctic.com 461 USER :Not enough parameters"; // ERR_NEEDMOREPARAMS
		if (parts[1][0] == '#')
			return ":irctic.com 432 * " + parts[1] + " :Erroneous username"; // ERR_ERRONEUSNICKNAME

		for (auto &c : server.getClients())
			if (c.username == parts[1])
				return ":irctic.com 464 * :Username is already in use"; // Custom error for duplicate username
		client.username = parts[1];
		if (client.updateAuthStatus())
				CompleteHandshake(clientId, server);
		return "Your username is now \"" + parts[1] + "\".";
	}



	else if (parts[0] == "NAMES") // LIST ALL PEOPLE / PEOPLE IN A CHANNEL
	{
		if (partsSize == 1)
		{
			std::string response = ":irctic.com 353 " + client.nickname + " = :"; // RPL_NAMREPLY
			for (auto &channel : server.getChannels())
			{
				response += channel.name + " :";
				for (auto &member : channel.getMembers())
				{
					if (member.second)
					{
						Client *cl = server.getClientById(member.first);
						if (cl)
							response += cl->nickname + " ";
					}
				}
				response += "\r\n";
			}
			response += ":irctic.com 366 " + client.nickname + " :End of NAMES list"; // RPL_ENDOFNAMES
			return response;
		}
		else if (partsSize >= 2)
		{
			Channel *channel = server.getChannel(parts[1]);
			if (!channel)
				return ":irctic.com 403 " + parts[1] + " :No such channel"; // ERR_NOSUCHCHANNEL
			std::string response = ":irctic.com 353 " + client.nickname + " = " + channel->name + " :";
			for (auto &member : channel->getMembers())
			{
				if (member.second)
				{
					Client *cl = server.getClientById(member.first);
					if (cl)
						response += cl->nickname + " ";
				}
			}
			response += "\r\n";
			response += ":irctic.com 366 " + client.nickname + " :End of NAMES list"; // RPL_ENDOFNAMES
			return response;
		}
	}



	else if (parts[0] == "JOIN") // JOIN OR CREATE A CHANNEL
	{
		if (!client.isAuthenticated)
			return ":irctic.com 451 JOIN :You have not registered"; // ERR_NOTREGISTERED
		if (partsSize < 2 || partsSize > 3)
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

		if (channel->password != "" && (partsSize < 3 || parts[2] != channel->password))
			return ":irctic.com 475 " + channelName + " :Cannot join channel (+k)\r\n"; // ERR_BADCHANNELKEY

		std::string channelJoinReturn = channel->addMember(clientId, server);
		client.channel = channel;

		return channelJoinReturn;
	}



	else if (parts[0] == "PART") // LEAVE A CHANNEL
	{
		if (!client.isAuthenticated)
			return ":irctic.com 451 PART :You have not registered"; // ERR_NOTREGISTERED
		if (!client.channel)
			return ":irctic.com 442 * :You're not on any channel"; // ERR_NOTONCHANNEL

		std::string partMessage = "PART " + client.channel->name;
		if (partsSize > 1)
			partMessage += " :" + parts[1];
		partMessage += "\r\n";

		client.channel->removeMember(clientId, server);
		client.channel = nullptr;

		return partMessage;
	}



	else if (parts[0] == "PRIVMSG" || parts[0] == "MSG") // MESSAGE PEOPLE
	{
		if (!client.isAuthenticated)
			return ":irctic.com 451 PRIVMSG :You have not registered"; // ERR_NOTREGISTERED
		if (partsSize < 3)
			return ":irctic.com 412 PRIVMSG :No text to send"; // ERR_NOTEXTTOSEND

		std::string target = parts[1];
		std::string msg = ":" + client.nickname + "!" + client.username + "@irctic.com PRIVMSG " + target + " :";
		for (size_t i = 2; i < parts.size(); i++)
		{
			msg += parts[i];
			if (i < parts.size() - 1)
				msg += " ";
		}
		msg += "\r\n"; 

		if (target[0] == '#')
		{
			Channel *channel = server.getChannel(target);
			if (!channel)
				return ":irctic.com 403 " + target + " :No such channel"; // ERR_NOSUCHCHANNEL
			channel->broadcast(msg, server, client.id);
		}
		else if (target != client.nickname)
		{
			std::cout << "Sending message to " << target << std::endl;
			Client *targetClientPtr = server.getClientByName(target);
			if (!targetClientPtr)
				return ":irctic.com 401 " + target + " :No such nick/channel"; // ERR_NOSUCHNICK
			if (!targetClientPtr->isAuthenticated)
				return ":irctic.com 401 " + target + " :No such nick/channel"; // ERR_NOSUCHNICK
			targetClientPtr->sendMessage(msg);
		}

		if (msg.back() == '\n')
			msg.pop_back();

		Logger::Log(LogLevel::INFO, std::string(client.getName() + " sent a message to " + target + ": " + msg));

		return std::string();
	}



	else if (parts[0] == "KICK")
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
		return ":irctic.com 200 KICK " + channel->name + " " + parts[2] + " :Kicked"; // Custom success reply
	}



	else if (parts[0] == "INVITE")
	{
		if (parts.size() != 3)
			return ":irctic.com 461 INVITE :Not enough parameters"; // ERR_NEEDMOREPARAMS
		Channel *channel = server.getChannel(parts[1]);
		if (!channel)
			return ":irctic.com 403 " + parts[1] + " :No such channel"; // ERR_NOSUCHCHANNEL
		if (!client.isOperatorIn(channel))
			return ":irctic.com 482 " + parts[1] + " :You're not channel operator"; // ERR_CHANOPRIVSNEEDED

		std::string userToInvite = parts[2];
		Client *clientToInvite = server.getClientByName(userToInvite);
		if (!clientToInvite)
			return ":irctic.com 401 " + userToInvite + " :No such nick/channel"; // ERR_NOSUCHNICK
		channel->unkick(clientToInvite->id);
		channel->addMember(clientToInvite->id, server);

		clientToInvite->sendMessage(":irctic.com INVITE " + userToInvite + " " + channel->name + " :You've been invited to the channel"); // RPL_INVITE
		return "Invited " + userToInvite + " to " + channel->name + ".";
	}



	else if (parts[0] == "TOPIC")
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



	else if (parts[0] == "MODE")
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



	return ":irctic.com 421 " + parts[0] + " :Unknown command"; // ERR_UNKNOWNCOMMAND
}
