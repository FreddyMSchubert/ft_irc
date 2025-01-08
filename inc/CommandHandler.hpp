#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "Client.hpp"

class Server; // forward declaration to avoid circular dependency

class CommandHandler
{
	public:
		static std::string HandleCommand(std::string inCommand, std::shared_ptr<Client> client, Server & server);
};
