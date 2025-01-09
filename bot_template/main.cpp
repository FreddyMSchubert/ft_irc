#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include "Bot.hpp"

int main(int argc, char *argv[])
{
    Bot bot("127.0.0.1", 8080, "password", "nick", "user");

    try {
        bot.connectToServer();
    } catch (std::exception& e) {
        std::cerr << "Failed to connect to IRC Server: " << e.what() << std::endl;
        return 1;
    }

    bot.Run();

    return 0;
}
