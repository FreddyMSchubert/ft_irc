#include "CommandHandler.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <sstream>
#include <vector>
#include <algorithm>

static std::vector<std::string> split(const std::string &str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim))
		tokens.push_back(token);
	return tokens;
}

void CommandHandler::handleCommand(Server &srv, int fd, const std::string &line)
{
	// Simple parsing example (more complex logic needed in real scenario)
	std::vector<std::string> parts = split(line, ' ');
	if (parts.empty()) return;

	// For comedic effect: "Mr. Elsher" cameo (◕‿◕)
	// Commands: PASS, NICK, USER, JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE, etc.

	if (parts[0] == "PASS")
	{
		if (parts.size() < 2) return;
		// Attempt auth
		if (srv.authenticateClient(parts[1]))
		{
			Client* c = srv.getClient(fd);
			if (c) c->setAuthenticated(true);
			srv.sendMessage(fd, "AUTH OK 🖕");
		}
		else
			srv.sendMessage(fd, "AUTH FAIL: Wrong password (ಥ﹏ಥ)");
	}
	else if (parts[0] == "NICK")
	{
		if (parts.size() < 2) return;
		Client* c = srv.getClient(fd);
		if (!c) return;
		c->setNickname(parts[1]);
		srv.sendMessage(fd, "Your nickname is now " + parts[1] + " ヽ(´▽`)/");
	}
	else if (parts[0] == "USER")
	{
		if (parts.size() < 2) return;
		Client* c = srv.getClient(fd);
		if (!c) return;
		c->setUsername(parts[1]);
		srv.sendMessage(fd, "Your username is now " + parts[1] + " (ಥ‿ಥ)");
	}
	else if (parts[0] == "JOIN")
	{
		// Example: JOIN #channel
		if (parts.size() < 2) return;
		Client* c = srv.getClient(fd);
		if (!c || !c->isAuthenticated()) 
		{
			srv.sendMessage(fd, "You must PASS first, dear friend (ಥ_ʖಥ)");
			return;
		}
		std::string channelName = parts[1];
		// If channel doesn't exist, create
		Channel *chan = srv.getChannel(channelName);
		if (!chan)
			chan = &srv.createChannel(channelName, fd);
		else
		{
			if (chan->getUserLimit() > 0 && (int)chan->getMembers().size() >= chan->getUserLimit())
			{
				srv.sendMessage(fd, "Channel is full 🚫");
				return;
			}
			chan->addMember(fd);
		}
		srv.broadcastChannel(channelName, c->getNickname() + " joined " + channelName, fd);
	}
	else if (parts[0] == "PRIVMSG")
	{
		// PRIVMSG #channel :Hello everyone!
		// or PRIVMSG nickname :Hello you
		if (parts.size() < 3) return;
		std::string target = parts[1];
		std::string msg;
		for (size_t i = 2; i < parts.size(); i++)
		{
			msg += parts[i];
			if (i < parts.size() - 1)
				msg += " ";
		}
		Client* c = srv.getClient(fd);
		if (target[0] == '#')
		{
			srv.broadcastChannel(target, c->getNickname() + ": " + msg, fd);
		}
		else
		{
			// private to user
			// find user by nickname
			std::map<int, Client> &allC = srv.getAllClients();
			for (std::map<int, Client>::iterator it = allC.begin(); it != allC.end(); ++it)
			{
				if (it->second.getNickname() == target)
				{
					srv.sendMessage(it->first, c->getNickname() + " (private): " + msg + " 🖕");
					break;
				}
			}
		}
	}
	else if (parts[0] == "KICK")
	{
		// KICK #channel user
		if (parts.size() < 3) return;
		Channel* chan = srv.getChannel(parts[1]);
		if (!chan) return;
		Client* c = srv.getClient(fd);
		if (!c->isAuthenticated() || !chan->isOperator(fd))
		{
			srv.sendMessage(fd, "You are not operator. Begone! (；一_一)");
			return;
		}
		// find user by nickname
		std::map<int, Client> &allC = srv.getAllClients();
		for (std::map<int, Client>::iterator it = allC.begin(); it != allC.end(); ++it)
		{
			if (it->second.getNickname() == parts[2])
			{
				if (chan->isMember(it->first))
				{
					chan->removeMember(it->first);
					srv.sendMessage(it->first, "You were KICKed from " + parts[1] + " 🦶👢");
					srv.broadcastChannel(parts[1], parts[2] + " was kicked from " + parts[1], fd);
					break;
				}
			}
		}
	}
	else if (parts[0] == "INVITE")
	{
		// INVITE user #channel
		if (parts.size() < 3) return;
		Channel* chan = srv.getChannel(parts[2]);
		if (!chan) return;
		if (!chan->isOperator(fd))
		{
			srv.sendMessage(fd, "You must be an operator to invite! ヽ(｀Д´)ﾉ");
			return;
		}
		// find user
		std::map<int, Client> &allC = srv.getAllClients();
		for (std::map<int, Client>::iterator it = allC.begin(); it != allC.end(); ++it)
		{
			if (it->second.getNickname() == parts[1])
			{
				srv.sendMessage(it->first, "You have been invited to " + parts[2] + " 🎉");
				// For an actual invite-only channel, you'd track invitations, etc.
				break;
			}
		}
	}
	else if (parts[0] == "TOPIC")
	{
		// TOPIC #channel newTopic
		if (parts.size() < 2) return;
		Channel* chan = srv.getChannel(parts[1]);
		if (!chan) return;
		Client* c = srv.getClient(fd);
		if (parts.size() == 2)
		{
			// Just get topic
			srv.sendMessage(fd, "Topic: " + chan->getTopic());
		}
		else
		{
			// set topic
			if (chan->isTopicRestricted() && !chan->isOperator(fd))
			{
				srv.sendMessage(fd, "Topic is restricted to ops only, dear chap!");
				return;
			}
			std::string newTopic;
			for (size_t i = 2; i < parts.size(); i++)
			{
				newTopic += parts[i];
				if (i < parts.size() - 1)
					newTopic += " ";
			}
			chan->setTopic(newTopic);
			srv.broadcastChannel(chan->getName(), c->getNickname() + " changed the topic to: " + newTopic);
		}
	}
	else if (parts[0] == "MODE")
	{
		// MODE #channel +i / -i / +o user / +k password, etc.
		// This is a fairly complicated command, so here's a snippet:
		if (parts.size() < 3) return;
		Channel* chan = srv.getChannel(parts[1]);
		if (!chan) return;
		Client* c = srv.getClient(fd);
		if (!c->isAuthenticated() || !chan->isOperator(fd))
		{
			srv.sendMessage(fd, "You are not channel operator! (ಠ_ಠ)");
			return;
		}
		std::string modes = parts[2];
		bool plus = (modes[0] == '+');
		for (size_t i = 1; i < modes.size(); i++)
		{
			switch (modes[i])
			{
				case 'i': chan->setInviteOnly(plus); break;
				case 't': chan->setTopicRestricted(plus); break;
				case 'k':
					if (plus && parts.size() > 3) chan->setPassword(parts[3]);
					else if (!plus) chan->setPassword("");
					break;
				case 'o':
					if (parts.size() > 3)
					{
						// find user by nickname
						std::map<int, Client> &allC = srv.getAllClients();
						for (std::map<int, Client>::iterator it = allC.begin(); it != allC.end(); ++it)
						{
							if (it->second.getNickname() == parts[3])
							{
								chan->setOperator(it->first, plus);
								srv.sendMessage(it->first, "You are now " + std::string(plus ? "" : "no longer ") + "an operator");
								break;
							}
						}
					}
					break;
				case 'l':
					if (plus && parts.size() > 3) chan->setUserLimit(atoi(parts[3].c_str()));
					else if (!plus) chan->setUserLimit(-1);
					break;
			}
		}
		srv.sendMessage(fd, "Mode change complete (∩｀-´)⊃━☆ﾟ.*･｡ﾟ");
	}
	else if (parts[0] == "FILE")
	{
		// Bonus: file transfer -> usage example: FILE #channel /path/to/file
		if (parts.size() < 3) return;
		srv.sendMessage(fd, "Initiating file transfer 🖕...");
		// In a real scenario: read file, send chunk by chunk to channel or specific user
		srv.getFileTransfer().transferFile(srv, fd, parts[1], parts[2]);
	}
	else if (parts[0] == "BOT")
	{
		// Bonus: BOT command
		// Example: BOT Hello there
		if (parts.size() < 2) return;
		// Our Bot will do something silly
		std::string content;
		for (size_t i = 1; i < parts.size(); i++)
			content += parts[i] + " ";
		srv.getBot().handleBotMessage(srv, fd, content);
	}
	else
	{
		// Unknown command
		srv.sendMessage(fd, "Unknown command, dear Mr. Elsher (◔_◔) 🖕");
	}
}
