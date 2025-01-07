#include "Server.hpp"
#include <cstdlib>
#include <iostream>

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>\n";
		return 1;
	}
	try
	{
		int port = std::atoi(av[1]);
		std::string password(av[2]);
		Server server(port, password);
		server.run(); // Start the main server loop (poll-based)
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}
