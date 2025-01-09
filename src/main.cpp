#include <iostream>
#include "../inc/Server.hpp"

#define DEFAULT_PORT 8080
#define DEFAULT_IP "127.0.0.1"
#define LOCALHOST "127.0.0.1"

int main(int argc , char *argv[])
{
	// TODO: get pwd and port from argv
	Server server(LOCALHOST, DEFAULT_PORT, "123");
	server.Run();
	return 0;
}