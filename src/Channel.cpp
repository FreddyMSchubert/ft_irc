#include "../inc/Channel.hpp"

#include "../inc/Server.hpp"

Channel::Channel(std::string name) : name(name) {};

std::string Channel::addMember(unsigned int clientId, Server &server, bool wasInvited)
{
	if (inviteOnly && !wasInvited)
		return "This channel is invite only.";
	if (_kicked[clientId])
		return "You have been kicked from this channel.";
	size_t currMemberInChannel = 0;
	for (const auto& member : _members)
		currMemberInChannel += member.second;
	if (limit > 0 && currMemberInChannel >= (size_t)limit)
		return std::string("This channel is full. [") + std::to_string(currMemberInChannel) + "/" + std::to_string(limit) + " members]";
	if (_members[clientId])
		return "You are already in this channel.";

	if (server.getClientById(clientId))
		server.getClientById(clientId)->channel = this;

	_members[clientId] = true;
	Logger::Log(LogLevel::INFO, std::string("Added client ") + server.getClientNameById(clientId) + " to channel " + name + ".");
	if (server.getClientById(clientId))
		broadcast(":" + server.getClientNameById(clientId) + "!" + server.getClientById(clientId)->username + "@irctic.com JOIN " + name, server, -1);
	return "You have joined " + name + ".";
}
void Channel::removeMember(unsigned int clientId, Server &server)
{
	std::cout << "Removing member " << clientId << " from channel " << name << std::endl;
	_members[clientId] = false;
	Client * client = server.getClientById(clientId); 
	if (client)
		client->channel = nullptr;
	if (server.getClientById(clientId))
		broadcast(":" + server.getClientNameById(clientId) + "!" + server.getClientById(clientId)->username + "@irctic.com PART " + name, server, -1);
}

void Channel::broadcast(std::string msg, Server &server, unsigned int except_id)
{
	for (const auto& member : _members)
	{
		unsigned int clientId = member.first;
		if (clientId != except_id && member.second)
		{
			Client* client = server.getClientById(clientId);
			if (client)
				client->sendMessage(msg);
		}
	}
}

void Channel::kick(unsigned int clientId, Server &server)
{
	removeMember(clientId, server);
	_kicked[clientId] = true;
}
void Channel::unkick(unsigned int clientId)
{
	_kicked[clientId] = false;
}

void Channel::addOperator(unsigned int clientId)
{
	_operators[clientId] = true;
}
void Channel::removeOperator(unsigned int clientId)
{
	_operators[clientId] = false;
}
bool Channel::isOperator(unsigned int clientId)
{
	return _operators[clientId];
}

std::map<unsigned int, bool> & Channel::getMembers()
{
	return _members;
}
