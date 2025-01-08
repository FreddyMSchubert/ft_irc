#pragma once

#include <string>

#include "Client.hpp"

class CommandHandler
{
	public:
		static std::string HandleCommand(std::string inCommand, Client & client);
};
