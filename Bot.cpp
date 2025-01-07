#include "Bot.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <cstdlib> // rand, srand
#include <ctime>
#include <vector>

// A trivial "bot" that responds with random witticisms
void Bot::handleBotMessage(Server &srv, int fd, const std::string &message)
{
	(void)message; // unused
	static bool seeded = false;
	if (!seeded)
	{
		std::srand((unsigned int)std::time(NULL));
		seeded = true;
	}

	std::vector<std::string> jokes;
	jokes.push_back("Why did the developer go broke? Because he used up all his cache! 🏦");
	jokes.push_back("I've got a really good UDP joke... but you might not get it. ( ͡° ͜ʖ ͡°)");
	jokes.push_back("In C++98, we don't talk about modern things like 'auto'—barbaric! (ง'̀-'́)ง");
	jokes.push_back("Never trust an atom, they make up everything. 🧪");
	jokes.push_back("You rang, Mr. Elsher? The Bot is at your service! 🍸");

	int r = std::rand() % jokes.size();
	// We'll just reply with a random quip
	srv.sendMessage(fd, "Bot says: " + jokes[r]);
}
