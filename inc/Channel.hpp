#pragma once

#include <string>
#include <memory>

#include "Client.hpp"

class Channel
{
	private:
		std::vector<std::weak_ptr<Client>> _members;

	public:
		Channel(std::string name);

		std::string name = "";

		void addMember(std::shared_ptr<Client> client);
		void broadcast(std::string msg);
};
