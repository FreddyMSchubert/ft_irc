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

		// Connections
		void updatePoll();
		void acceptNewConnections();
		void handleExistingConnections();

		// Irc Logic
		void HandleClientData(Client & client);

	public:
		Server(int port);
		Server(Server const &src) = delete;
		Server &operator=(Server const &src) = delete;
		Server(Server&& other) noexcept = default;
		~Server() = default;
		
		void Run(); // Main Server loop
};
