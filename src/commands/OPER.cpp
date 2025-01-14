#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleOPER(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 3)
		return ":irctic.com 461 OPER :Not enough parameters"; // ERR_NEEDMOREPARAMS
	if (!client.isAuthenticated)
		return ":irctic.com 451 * :You have not registered"; // ERR_NOTREGISTERED

	Client *target = server.getClientByName(parts[1]);
	if (!target)
		return ":irctic.com 401 " + parts[1] + " :No such nick/channel"; // ERR_NOSUCHNICK
	if (server.isCorrectOperatorPassword(parts[2]))
	{
		target->isOperator = true;
		return ":irctic.com 381 * :" + target->nickname + " is now an IRC operator"; // RPL_YOUREOPER
	}
	std::cout << "Typed operator password: \"" << parts[2] << "\"" << std::endl;
	return ":irctic.com 464 * :Operator password incorrect"; // ERR_PASSWDMISMATCH
}
