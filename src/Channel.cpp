#include "../inc/Channel.hpp"

#include "../inc/Server.hpp"

Channel::Channel(std::string name) : name(name) {};

void Channel::addMember(unsigned int clientId, Server &server)
{
	for (const auto& member : _members)
		if (member == clientId)
			return; // Already a member

	_members.emplace_back(clientId);
	Logger::Log(LogLevel::INFO, std::string("Added client ") + server.getClientNameById(clientId) + " to channel " + name + ".");
}

void Channel::broadcast(std::string msg, Server &server, unsigned int except_id)
{
	for (auto& member : _members)
	{
		if (member != except_id)
		{
			Client * client = server.getClientById(member);
			if (client)
				client->outbuffer += msg;
		}
	}
}

void Channel::removeClient(unsigned int clientId)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == clientId)
		{
			_members.erase(_members.begin() + i);
			return;
		}
	}
}
