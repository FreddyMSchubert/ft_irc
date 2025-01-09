#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include "Bot.hpp"

int main(int argc, char *argv[])
{
    // const char* SERVER_IP = "127.0.0.1";
    // const int SERVER_PORT = std::stoi(argv[1]);

    // int sock_fd;
    // struct sockaddr_in server_addr;

    // if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	// {
    //     perror("Socket creation failed");
    //     exit(EXIT_FAILURE);
    // }

    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(SERVER_PORT);
    // if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
	// {
    //     perror("Invalid address/ Address not supported");
    //     close(sock_fd);
    //     exit(EXIT_FAILURE);
    // }

    // if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	// {
    //     perror("Connection to server failed");
    //     close(sock_fd);
    //     exit(EXIT_FAILURE);
    // }

    // std::cout << "Connected to the server!" << std::endl;

    // const std::string message = "Hello, Server!";
    // if (send(sock_fd, message.c_str(), message.length(), 0) == -1)
    //     perror("Message send failed");
    // else
    //     std::cout << "Message sent: " << message << std::endl;
    
    // close(sock_fd);
    // return 0;

    Bot bot();
    return 0;
}
