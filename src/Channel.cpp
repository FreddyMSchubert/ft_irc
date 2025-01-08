#include "../inc/Channel.hpp"

Channel::Channel(std::string name) : name(name) {};

void Channel::addMember(std::shared_ptr<Client> client)
{
	for (const auto& member_wp : _members)
	{
		if (auto member_sp = member_wp.lock())
		{
			if (member_sp == client)
				return; // Already a member
		}
	}

	_members.emplace_back(client);
	Logger::Log(LogLevel::INFO, std::string("Added client ") + client->getName() + " to channel " + name + ".");
}

void Channel::broadcast(std::string msg)
{
	for (const auto& member_wp : _members)
		if (auto member_sp = member_wp.lock())
			member_sp->outbuffer += msg;
}
