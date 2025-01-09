#include "../inc/Channel.hpp"

#include "../inc/Server.hpp"

Channel::Channel(std::string name) : name(name) {};

std::string Channel::addMember(unsigned int clientId, Server &server, bool wasInvited)
{
	if (inviteOnly && !wasInvited)
		return "This channel is invite only.";
	if (std::find(_kicked.begin(), _kicked.end(), clientId) != _kicked.end())
		return "You have been kicked from this channel.";
	for (const auto& member : _members)
		if (member == clientId)
			return "You are already in this channel.";

	if (server.getClientById(clientId))
		server.getClientById(clientId)->channel = this;

	_members.emplace_back(clientId);
	Logger::Log(LogLevel::INFO, std::string("Added client ") + server.getClientNameById(clientId) + " to channel " + name + ".");
	return "You have joined " + name + ".";
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

void Channel::removeClient(unsigned int clientId, Server &server)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == clientId)
		{
			Client * client = server.getClientById(clientId);
			_members.erase(_members.begin() + i);
			if (client)
				client->channel = nullptr;
			return;
		}
	}
}

void Channel::kick(unsigned int clientId, Server &server)
{
	removeClient(clientId, server);
	_kicked.emplace_back(clientId);
}
void Channel::unkick(unsigned int clientId)
{
	for (size_t i = 0; i < _kicked.size(); i++)
	{
		if (_kicked[i] == clientId)
		{
			_kicked.erase(_kicked.begin() + i);
			return;
		}
	}
}
