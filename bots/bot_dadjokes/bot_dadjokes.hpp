#ifndef BOT_DADJOKES_HPP
#define BOT_DADJOKES_HPP

#include "../Bot.hpp"
#include <string>
#include <curl/curl.h>
#include <iostream>

class Bot_DadJokes : public Bot
{
	public:
		Bot_DadJokes(
			const std::string& host,
			int port,
			const std::string& password,
			const std::string& user,
			const std::string& realName
		);

	std::string ApiCall();
};

#endif