#ifndef FILETRANSFER_HPP
#define FILETRANSFER_HPP

#include <string>

class Server;

class FileTransfer
{
public:
	FileTransfer() {}
	~FileTransfer() {}

	void transferFile(Server &srv, int senderFd, const std::string &target, const std::string &filePath);
};

#endif
