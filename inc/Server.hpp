#pragma once

#include "Socket.hpp"
#include "Client.hpp"
#include "CommandHandler.hpp"
#include "./Utils/Logger.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <chrono>

class Server
{
	private:
		int							_port;
		Client						_listening_socket;
		std::vector<Client>			_sockets;

		std::string					_password;
		std::string					_op_password;

		void updatePoll();
		void acceptNewConnections();
		void handleExistingConnections();

		void HandleClientData(Client & client);

	public:
		Server(int port, std::string _password, std::string op_password);
		Server(Server const &src) = delete;
		Server &operator=(Server const &src) = delete;
		Server(Server&& other) noexcept = default;
		~Server() = default;
		
		void Run(); // Main Server loop

		bool isCorrectPassword(std::string passwordAttempt);
		bool isCorrectOperatorPassword(std::string passwordAttempt);
};
