#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleCAP(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)client;
	(void)server;

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
