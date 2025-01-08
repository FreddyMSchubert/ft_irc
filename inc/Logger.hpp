#pragma once

#include <iostream>

class Logger
{
	private:
		enum class LogLevel
		{
			INFO,
			WARNING,
			ERROR
		};
		static void Log(LogLevel level, const std::string &message);

	public:
		static void Error(const std::string &message);
		static void Warning(const std::string &message);
		static void Info(const std::string &message);
};