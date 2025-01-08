#pragma once

#include "Socket.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "CommandHandler.hpp"
#include "./Utils/Logger.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <chrono>
#include <memory>

class Server
{
	private:
		int										_port;
		Client									_listening_socket;
		std::vector<std::shared_ptr<Client>>	_sockets;

		std::string					_password;
		std::string					_op_password;
		std::vector<Channel>		_channels;

		void updatePoll();
		void acceptNewConnections();
		void handleExistingConnections();

		void HandleClientData(std::shared_ptr<Client> & client);

	public:
		Server(int port, std::string _password, std::string op_password);
		Server(Server const &src) = delete;
		Server &operator=(Server const &src) = delete;
		Server(Server&& other) noexcept = default;
		~Server() = default;
		
		void Run(); // Main Server loop

		bool isCorrectPassword(std::string passwordAttempt);
		bool isCorrectOperatorPassword(std::string passwordAttempt);

		Channel *getChannel(std::string channelName);
		void createChannel(std::string channelName);
};
