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

std::string CommandHandler::HandleCommand(std::string inCommand, unsigned int clientId, Server & server)
{
	Client *clientPtr = server.getClientById(clientId);
	if (!clientPtr)
		return "Client not found";
	Client &client = *clientPtr;

	std::vector<std::string> parts = split(inCommand, ' ');
	int partsSize = parts.size();
	if (parts.empty())
		return "Error reading command";

	if (parts[0] == "PASS") // AUTHENTICATE
	{
		if (partsSize != 2)
			return std::string("Format: \"PASS <password>\".") + (client.isAuthenticated ? " You are authenticated. " : " You are not authenticated.");
		if (client.isAuthenticated)
			return "You are already authenticated.";
		if (client.username.empty())
			return "Please set a username using USER before authenticating.";
		if (client.nickname.empty())
			return "Please set a nickname using NICK before authenticating.";

		if (server.isCorrectPassword(parts[1]))
		{
			client.isAuthenticated = true;
			return ":irctic.com 001 " + client.nickname + " :Welcome to the IRCtic, " + client.nickname + "!\n";
		}
		return "Authentication failed.";
	}

	else if (parts[0] == "CAP")
	{
		if (parts.size() >= 2)
		{
			if (parts[1] == "LS")
				return ":irctic.com CAP * LS :";
			else if (parts[1] == "REQ")
				return ":irctic.com CAP * ACK :";
		}
		return ":irctic.com CAP * END :";
	}

	else if (parts[0] == "PING")
	{
		if (partsSize < 2)
			return "PONG";
		return "PONG " + parts[1];
	}

	else if (parts[0] == "PONG")
	{
		if (partsSize < 2)
			return "PING";
		return "PING " + parts[1];
	}

	else if (parts[0] == "OPER") // AUTHENTICATE AS OPERATOR
	{
		if (partsSize != 2)
			return std::string("Format: \"OPER <operator password>\".") + (client.isOperator ? " You are an operator. " : " You are not an operator.");
		if (client.isOperator)
			return "You are already an operator.";
		if (client.username.empty())
			return "Please set a username using USER before authenticating.";
		if (client.nickname.empty())
			return "Please set a nickname using NICK before authenticating.";

		if (server.isCorrectOperatorPassword(parts[1]))
		{
			client.isOperator = true;
			client.isAuthenticated = true;
			return "Operator Authentication successful.";
		}
		return "Operator Authentication failed.";
	}

	else if (parts[0] == "NICK") // SET NICKNAME
	{
		if (partsSize != 2)
			return std::string("Format: \"NICK <new nickname>\".") + (client.nickname.empty() ? " You currently don't have a nickname." : " Your current nickname is: \"" + client.nickname + "\".");
		if (parts[1][0] == '#')
			return "Nicknames can't start with \"#\"";

		for (auto &c : server.getClients())
			if (c.nickname == parts[1])
				return "Nickname already in use.";
		client.nickname = parts[1];
		return "Your nickname is now \"" + parts[1] + "\".";
	}

	else if (parts[0] == "USER") // SET USERNAME
	{
		if (partsSize != 2)
			return std::string("Format: \"USER <new username>\".") + (client.username.empty() ? " You currently don't have a username." : " Your current username is: \"" + client.username + "\".");
		if (parts[1][0] == '#')
			return "Usernames can't start with \"#\"";

		for (auto &c : server.getClients())
			if (c.username == parts[1])
				return "Username already in use.";
		client.username = parts[1];
		return "Your username is now \"" + parts[1] + "\".";
	}
	else if (parts[0] == "JOIN") // JOIN OR CREATE A CHANNEL
	{
		if (!client.isAuthenticated)
			return "Please authenticate using PASS before joining a channel.";
		if (partsSize < 2 || partsSize > 3)
		{
			std::string response = "Format: \"JOIN <channel name> <channel password if necessary>\".\n";
			if (client.channel)
				response += "You are currently in channel " + client.channel->name + ".\n";
			if (server.getChannels().size() > 0)
			{
				response += "Joinable channels:   ";
				for (auto &channel : server.getChannels())
					response += channel.name + ", ";
				response.pop_back();
				response.pop_back();
			}
			return response;
		}
		
		std::string channelName = parts[1];
		Channel *channel = server.getChannel(channelName);
		if (!channel)
		{
			if (channelName[0] != '#')
				return "Channel names must start with \"#\"";
			server.createChannel(channelName);
			channel = server.getChannel(channelName);
		}

		if (channel->password != "" && (partsSize < 3 || parts[2] != channel->password))
			return "Incorrect or missing password for channel " + channel->name + ".";

		std::string channelJoinReturn = channel->addMember(clientId, server);
		client.channel = channel;

		return channelJoinReturn;
	}

	else if (parts[0] == "PRIVMSG" || parts[0] == "MSG") // MESSAGE PEOPLE
	{
		if (!client.isAuthenticated)
			return "Please authenticate using PASS before messaging people.";
		if (partsSize < 3)
			return "Format: \"PRIVMSG <channel name or person nickname> <message>";

		std::string target = parts[1];
		std::string msg = client.getName() + ": ";
		for (size_t i = 2; i < parts.size(); i++)
		{
			msg += parts[i];
			if (i < parts.size() - 1)
				msg += " ";
		}
		msg += "\n";

		if (target[0] == '#')
		{
			Channel *channel = server.getChannel(target);
			if (!channel)
				return ("Channel not found");
			channel->broadcast(msg, server, client.id);
		}
		else if (target != client.nickname)
		{
			std::cout << "Sending message to " << target << std::endl;
			std::vector<Client> &clients = server.getClients();
			for (auto &c : clients)
			{
				std::cout << "Checking " << c.nickname << std::endl;
				if (c.nickname == target)
				{
					if (!c.isAuthenticated)
						return "Message not sent. The target is not authenticated.";
					c.sendMessage(msg);
				}
			}
		}

		return std::string();
	}

	else if (parts[0] == "KICK")
	{
		if (parts.size() != 3)
			return "Format: \"KICK <channel name> <client nickname>\"";
		Channel *channel = server.getChannel(parts[1]);
		if (!channel)
			return "Channel not found";
		if (!client.isOperatorIn(channel))
			return "You must be an operator to kick someone.";

		std::string userToKick = parts[2];
		unsigned int clientIdToKick = 0;
		for (auto &c : server.getClients())
			if (c.nickname == userToKick)
				clientIdToKick = c.id;
		channel->kick(clientIdToKick, server);

		if (server.getClientById(clientIdToKick))
			server.getClientById(clientIdToKick)->sendMessage("You have been kicked from " + channel->name + ".\n");
		return "Kicked " + userToKick + " from " + channel->name + ".";
	}

	else if (parts[0] == "INVITE")
	{
		if (parts.size() != 3)
			return "Format: \"INVITE <channel name> <client nickname>\"";
		Channel *channel = server.getChannel(parts[1]);
		if (!channel)
			return "Channel not found";
		if (!client.isOperatorIn(channel))
			return "You must be an operator to invite someone.";

		std::string userToInvite = parts[2];
		unsigned int clientIdToInvite = 0;
		for (auto &c : server.getClients())
			if (c.nickname == userToInvite)
				clientIdToInvite = c.id;
		channel->unkick(clientIdToInvite);
		channel->addMember(clientIdToInvite, server);

		if (server.getClientById(clientIdToInvite))
			if (!server.getClientById(clientIdToInvite)->isAuthenticated)
				return "Invited " + userToInvite + " to " + channel->name + ". Be warned that they are not authenticated.";

		if (server.getClientById(clientIdToInvite))
			server.getClientById(clientIdToInvite)->sendMessage("You have been invited to " + channel->name + ".\n");
		return "Invited " + userToInvite + " to " + channel->name + ".";
	}

	else if (parts[0] == "TOPIC")
	{
		if (parts.size() != 3)
		{
			if (client.channel && client.channel->topic != "")
				return std::string("Format: \"TOPIC <channel name> <new topic>\" - Current topic: ") + client.channel->topic;
			return "Format: \"TOPIC <channel name> <new topic>\"";
		}

		Channel *channel = server.getChannel(parts[1]);
		if (!channel)
			return "Channel not found";
		if (!client.isOperatorIn(channel) && !channel->anyoneCanChangeTopic)
			return "You must be an operator to change the topic.";
		
		channel->topic = parts[2];
		channel->broadcast("Topic changed to: " + parts[2] + "\n", server);
		return (client.channel && client.channel == channel) ? "" : "Topic changed to: " + parts[2] + " in " + channel->name;
	}

	else if (parts[0] == "MODE")
	{
		if (parts.size() < 3 || parts.size() > 4)
			return "Format: \"MODE <channel name> <mode> <optional argument>\"";
		Channel *channel = server.getChannel(parts[1]);
		if (!client.isOperatorIn(channel))
			return "You must be an operator to change the mode.";
		if (!channel)
			return "Channel not found";
		
		std::string mode = parts[2];
		if (mode == "+i")
			channel->inviteOnly = true;
		else if (mode == "-i")
			channel->inviteOnly = false;
		else if (mode == "+t")
			channel->anyoneCanChangeTopic = true;
		else if (mode == "-t")
			channel->anyoneCanChangeTopic = false;
		else if (mode == "+k")
		{
			if (parts.size() != 4)
				return "Format: \"MODE <channel name> +k <password>\"";
			channel->password = parts[3];
		}
		else if (mode == "-k")
			channel->password = "";
		else if (mode == "+l")
		{
			if (parts.size() != 4)
				return "Format: \"MODE <channel name> +l <limit>\"";
			channel->limit = std::stoi(parts[3]);
		}
		else if (mode == "-l")
			channel->limit = 0;
		else if (mode == "+o")
		{
			if (parts.size() != 4)
				return "Format: \"MODE <channel name> +o <client nickname>\"";
			unsigned int clientIdToOp = server.getClientIdByName(parts[3]);
			if (clientIdToOp > 0)
				channel->addOperator(clientIdToOp);
			else
				return "Client not found.";
		}
		else if (mode == "-o")
		{
			if (parts.size() != 4)
				return "Format: \"MODE <channel name> -o <client nickname>\"";
			unsigned int clientIdToDeop = server.getClientIdByName(parts[3]);
			if (clientIdToDeop > 0)
				channel->removeOperator(clientIdToDeop);
			else
				return "Client not found.";
		}
		else
			return "Unrecognized mode. Available modes: +i, -i, +t, -t, +k, -k, +l, -l, +o, -o";
		
		return "Mode set.";
	}

	else if (parts[0] == "USERS")
	{
		// log all users
		std::string response = "Users:\n";
		for (auto &c : server.getClients())
			response += "- " + c.getName() + " (id" + std::to_string(c.id) + ")\n";
		response.pop_back();
		return response;
	}

	return "Unrecognized command. Available commands: PASS, OPER, NICK, USER, JOIN, PRIVMSG / MSG, KICK, INVITE, TOPIC, MODE, USERS";
}
