#ifndef FILETRANSFER_HPP
#define FILETRANSFER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <map>

class Server;

class FileTransfer
{
	public:
		FileTransfer() {}
		~FileTransfer() {}

		void transferFile(Server &server, int senderFd, const std::string &nickname, const std::string &filePath);
};

#endif
