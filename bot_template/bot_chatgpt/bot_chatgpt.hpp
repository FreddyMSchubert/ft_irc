#pragma once

#include "../Bot.hpp"
#include <string>
#include <curl/curl.h>

class Bot_ChatGPT : public Bot
{
	public:
		Bot_ChatGPT(const std::string &host,
					int port,
					const std::string &password,
					const std::string &user,
					const std::string &realName);

	std::string ApiCall(const std::string &prompt);
};