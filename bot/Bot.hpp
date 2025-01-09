#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <regex>
#include <vector>
#include <sstream>

class Server;

//TODO: make listening-sockets (copy from src)
//TODO: make server (copy from src)

class Bot
{
	private:
		//attributes to know whom to send what and how
		std::string _username_player1;
		std::string _username_player2;
		bool		_send_back_as_privmsg;
		bool		_send_back_in_channel;
		bool		_game_is_won = true;

		//tictactoe-attributes
		std::vector<char>	_board;
		char				_currentPlayer;
		bool				_gameOver;
		int					_movesCount;

		//tictactoe-functions
		void		TicTacToe(int move);
		void		ticTacToeReset();
		bool		checkWin();
		std::string getBoardStr() const;
		
		//API
		std::string ApiCall();

	public:
		Bot() {}
		~Bot() {}

		//Parser
		void parseInput(std::string);
};

#endif
