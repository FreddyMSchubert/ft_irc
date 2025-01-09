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
		if (server.isCorrectPassword(parts[1]))
		{
			client.isAuthenticated = true;
			return "Authentication successful.";
		}
		return "Authentication failed.";

	}

	if (parts[0] == "OPER") // AUTHENTICATE AS OPERATOR
	{
		if (partsSize != 2)
			return std::string("Format: \"OPER <operator password>\".") + (client.isOperator ? " You are an operator. " : " You are not an operator.");
		if (client.isOperator)
			return "You are already an operator.";
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
		client.nickname = parts[1];
		return "Your nickname is now \"" + parts[1] + "\".";
	}

	else if (parts[0] == "USER") // SET USERNAME
	{
		if (partsSize != 2)
			return std::string("Format: \"USER <new username>\".") + (client.username.empty() ? " You currently don't have a username." : " Your current username is: \"" + client.username + "\".");
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
		channel->addMember(clientId, server);
		client.channel = channel;

		return "You have joined " + channelName + ".";
	}

	else if (parts[0] == "PRIVMSG") // MESSAGE PEOPLE
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
			std::vector<Client> &clients = server.getClients();
			for (auto &c : clients)
				if (c.nickname == target)
					c.outbuffer += msg;
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
		if (!client.isOperator)
			return "You must be an operator to kick someone.";

		std::string userToKick = parts[2];
		unsigned int clientIdToKick = 0;
		for (auto &c : server.getClients())
			if (c.nickname == userToKick)
				clientIdToKick = c.id;
		channel->kick(clientIdToKick, server);

		if (server.getClientById(clientIdToKick))
			server.getClientById(clientIdToKick)->outbuffer += "You have been kicked from " + channel->name + ".\n";
		return "Kicked " + userToKick + " from " + channel->name + ".";
	}

	return "Unrecognized command. Available commands: PASS, OPER, NICK, USER, JOIN, MSG, ";
}
