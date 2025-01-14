#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() > 1)
		return "QUIT :" + parts[1] + "\r\n";
	std::vector<Client> &clients = server.getClients();
	for (auto it = clients.begin(); it != clients.end(); it++)
	{
		if (it->id == client.id)
		{
			clients.erase(it);
			break;
		}
	}
	return "QUIT\r\n";
}
