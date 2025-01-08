#include "../inc/FileTransfer.hpp"
// #include "../core/Server.hpp"
// #include "../modules/Client.hpp"

// void FileTransfer::transferFile(Server &server, int senderFd, const std::string &target, const std::string &filepath)
// {
// 	std::string file;

// 	//open file and read it
// 	std::ifstream filestream(filepath.c_str());
// 	if (!filestream.is_open())
// 		return (server.sendMessage(senderFd, "Could not open file for reading. (；一_一)"));
// 	while (getline(filestream, file))
// 		std::cout << file << std::endl;
// 	filestream.close();

// 	//check if its a broadcast with # otherwise check if there is a user == target and send it to him
// 	if (target[0] == '#')
// 		return (server.broadcastChannel(target, "File content from user " + server.getClient(senderFd)->getUsername() + ":\n" + file)); //under the assumption that username has a default value like anonymus or smth.
// 	else
// 	{
// 		std::map<int, Client> &allClients = server.getAllClients();
// 		for (const auto &pair : allClients)
// 			if (pair.second.getUsername() == target)
// 				return (server.sendMessage(pair.first, "File content from user " + server.getClient(senderFd)->getUsername() + ":\n" + file));
// 	}
// 	server.sendMessage(senderFd, "Either the user or the channel doesn't exist! (ㆆ_ㆆ)");
// }
