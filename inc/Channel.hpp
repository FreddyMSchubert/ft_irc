#pragma once

#include <string>
#include <memory>
#include <map>

#include "Client.hpp"

class Server;

class Channel
{
	private:
		std::map<unsigned int, bool> _members;
		std::map<unsigned int, bool> _kicked;
		std::map<unsigned int, bool> _operators;

	public:
		Channel(std::string name);

		std::string name = "";
		std::string topic = "";
		std::string password = "";
		bool inviteOnly = false;
		bool anyoneCanChangeTopic = true;
		int limit = 0;

		void broadcast(std::string msg, Server &server, unsigned int except_id = -1);

		std::string addMember(unsigned int client, Server &server, bool wasInvited = false);
		void removeMember(unsigned int clientId, Server &server);
	
		void kick(unsigned int clientId, Server &server);
		void unkick(unsigned int clientId);

		void addOperator(unsigned int clientId);
		void removeOperator(unsigned int clientId);
		bool isOperator(unsigned int clientId);
};
