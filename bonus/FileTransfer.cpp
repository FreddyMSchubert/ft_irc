#include "FileTransfer.hpp"
#include "../core/Server.hpp"
#include "../modules/Client.hpp"
#include <iostream>
#include <fstream>
#include <map>

void FileTransfer::transferFile(Server &srv, int senderFd, const std::string &target, const std::string &filePath)
{
	std::ifstream file(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		srv.sendMessage(senderFd, "Could not open file for reading. (；一_一)");
		return;
	}
	// For demonstration, we read the entire file into a buffer (not recommended for large files!)
	std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// If target is a channel, broadcast. Otherwise, direct to user with nickname == target
	if (target[0] == '#')
	{
		srv.broadcastChannel(target, "File content from FD " + std::to_string(senderFd) + ":\n" + data);
	}
	else
	{
		std::map<int, Client> &allC = srv.getAllClients();
		for (std::map<int, Client>::iterator it = allC.begin(); it != allC.end(); ++it)
		{
			if (it->second.getNickname() == target)
			{
				srv.sendMessage(it->first, "File content from FD " + std::to_string(senderFd) + ":\n" + data);
				break;
			}
		}
	}
	srv.sendMessage(senderFd, "File transfer complete! (ﾉ◕ヮ◕)ﾉ*:･ﾟ✧");
}
