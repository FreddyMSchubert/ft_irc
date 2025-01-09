#pragma once

#include <string>
#include <memory>

#include "Client.hpp"

class Server;

class Channel
{
	private:
		std::vector<unsigned int> _members;
		std::vector<unsigned int> _kicked;

	public:
		Channel(std::string name);

		std::string name = "";

		void addMember(unsigned int client, Server &server);
		void broadcast(std::string msg, Server &server, unsigned int except_id = -1);
		void removeClient(unsigned int clientId, Server &server);
		void kick(unsigned int clientId, Server &server);
};
