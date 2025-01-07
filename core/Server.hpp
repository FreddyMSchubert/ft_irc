#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>
#include <poll.h>
#include "../modules/Client.hpp"
#include "../modules/Channel.hpp"
#include "../commands/CommandHandler.hpp"
#include "../bonus/Bot.hpp"
#include "../bonus/FileTransfer.hpp"

class Server
{
private:
	int                 _port;
	std::string         _password;
	int                 _listener;
	bool                _running;

	std::vector<struct pollfd>        _pollFds;
	std::map<int, Client>             _clients;
	std::map<std::string, Channel>    _channels;

	CommandHandler       _cmdHandler;
	Bot                  _bot;
	FileTransfer         _fileTransfer;

	void setupListener();
	void acceptNewClient();
	void handleClientData(int fd);
	void disconnectClient(int fd);

public:
	Server(int port, const std::string &password);
	~Server();

	void run();

	// Helper functions
	bool authenticateClient(const std::string &passAttempt);
	void sendMessage(int fd, const std::string &msg);
	void broadcastChannel(const std::string &channelName, const std::string &msg, int excludeFd = -1);
	
	// Channel access
	Channel* getChannel(const std::string &chanName);
	Channel& createChannel(const std::string &chanName, int creatorFd);
	void removeEmptyChannel(const std::string &chanName);

	// Client manipulation
	Client* getClient(int fd);
	std::map<int, Client> &getAllClients();
	Bot& getBot();
	FileTransfer& getFileTransfer();
};

#endif
