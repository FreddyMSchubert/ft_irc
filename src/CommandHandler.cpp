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

std::string CommandHandler::HandleCommand(std::string inCommand, Client & client, Server & server)
{
	std::vector<std::string> parts = split(inCommand, ' ');
	int partsSize = parts.size();
	if (parts.empty())
		return "Error reading command";

	if (parts[0] == "PASS") // AUTHENTICATE
	{
		if (partsSize != 2)
			return std::string("Format: \"PASS <password>\".") + (client.isAuthenticated ? " You are authenticated. " : " You are not authenticated.");
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
		if (server.isCorrectPassword(parts[1]))
		{
			client.isOperator = true;
			return "Operator Authentication successful.";
		}
		return "Operator Authentication failed.";
	}

	else if (parts[0] == "NICK") // SET NICKNAME
	{
		if (partsSize != 2)
			return std::string("Format: \"NICK <new nickname>\".") + (client.nickname.empty() ? " You currently don't have a nickname." : " Your current nickname is: \"" + client.nickname + "\".");
		client.nickname = parts[1];
		return "Your nickname is now \"" + parts[1] + "\".";
	}

	else if (parts[0] == "USER") // SET USERNAME
	{
		if (partsSize != 2)
			return std::string("Format: \"NICK <new username>\".") + (client.username.empty() ? " You currently don't have a username." : " Your current username is: \"" + client.username + "\".");
		client.username = parts[1];
		return "Your username is now \"" + parts[1] + "\".";
	}

	else if (parts[0] == "JOIN") // JOIN OR CREATE A CHANNEL
	{
		if (partsSize < 2 || partsSize > 3)
			return "Format: \"JOIN <channel name> <channel password if necessary>\"";
		if (!client.isAuthenticated)
			return "Please authenticate using PASS before joining a channel.";
		
		std::string channelName = parts[1];
	}

	return "Unrecognized command. Available commands: PASS, OPER, NICK, USER, JOIN, ";
}
