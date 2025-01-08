#include "../inc/Utils/Logger.hpp"
#include "../inc/Server.hpp"

#include <string>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		Logger::Log(LogLevel::ERROR, std::string("Incorrect arguments. Usage: ") + argv[0] + " <port> <password>");
		return -1;
	}

	int port = -1;
	std::string password = argv[2];
	try
	{
		port = std::atoi(argv[1]);
	}
	catch (const std::exception &e)
	{
		Logger::Log(LogLevel::ERROR, std::string("Problem parsing arguments: ") + e.what());
	}
	if (port < 0)
	{
		Logger::Log(LogLevel::ERROR, "Trouble parsing port");
	}

	Server cm(port);
	cm.Run();

	return 0;
}
