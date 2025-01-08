#include "Bot.hpp"
#include <curl/curl.h>
//input parsing and helper functions
std::vector<const std::string> split(const std::string& str, char delimiter)
{
    std::vector<const std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter))
        result.push_back(item);
    return result;
}

//allowed:
//BOT MSG player1 player2
//BOT #topic player1 player2
//BOT dadjokes
//BOT help
//player 1 is the username of the player that want to challenge the other person (player2)
void Bot::parseInput(std::string input)
{
	std::regex private_match("^BOT MSG (\\w+) (\\w+)$");
    std::regex channel_match("^BOT #(\\w+) (\\w+) (\\w+)$");
	std::regex game_move("^BOT MSG (\w+) ([1-9])$");
	std::regex dad_jokes("^BOT dadjokes$");
	std::regex help("^BOT help");

	if (std::regex_match(input, private_match) || std::regex_match(input, channel_match) && _game_is_won)
	{
		std::vector<const std::string> input_split = split(input, ' ');
        input_split.erase(std::remove_if(input_split.begin(), input_split.end(), [](const std::string& str) { return str == " "; }), input_split.end());
		_username_player1 = input_split[2];
		_username_player2 = input_split[2];
		if (input.find('#'))
			_send_back_in_channel = true;
		else
			_send_back_as_privmsg = true;
		_game_is_won = false;
		ticTacToeReset();
		//send back a message that the game has started
	}
	else if (std::regex_match(input, game_move))
	{
		//gamelogic
		//input must be an int need to extract that
		TicTacToe(3);
	}
	else if (std::regex_match(input, dad_jokes))
	{
		std::string response = ApiCall();
		//send message back to person with the string
	}
	else if (std::regex_match(input, help))
	{
		//message to player: all the commands that are available
	}
	else
	{
		//message to player: fuck off wrong cmd or type help
	}
}

//tic tac toe and helper functions
void Bot::ticTacToeReset()
{
	_board = {'1','2','3','4','5','6','7','8','9'};
	_currentPlayer = 'X';
	_gameOver = false;
	_movesCount = 0;
}

std::string Bot::getBoardStr() const
{
	std::ostringstream oss;
	for (int i = 0; i < 9; i += 3)
	{
		oss << _board[i] << " | " << _board[i+1] << " | " << _board[i+2] << "\n";
		if (i < 6)
			oss << "--+---+--\n";
	}
	return oss.str();
}

bool Bot::checkWin()
{
	static int winCombos[8][3] = {{0,1,2}, {3,4,5}, {6,7,8}, {0,3,6}, {1,4,7}, {2,5,8}, {0,4,8}, {2,4,6}};
	for (auto &combo : winCombos)
		if (_board[combo[0]] == _board[combo[1]] && _board[combo[1]] == _board[combo[2]])
			return true;
	return false;
}

void Bot::TicTacToe(int pos)
{
	if (pos < 1 || pos > 9)
		//send Message to player that this move is illegal
	if (_gameOver)
		//send Message that the game is over
	int index = pos - 1;
	if (_board[index] == 'X' || _board[index] == 'O')
		//send Message to player that this square is already taken
	_board[index] = _currentPlayer;
	_movesCount++;
	if (checkWin())
	{
		_gameOver = true;
		_game_is_won = true;
		//send Message for both that we have a winner
	} 
	else if (_movesCount == 9)
		_gameOver = true;
		//send Message to both that it's a draw
	_currentPlayer = (_currentPlayer == 'X') ? 'O' : 'X';
	//send Message to both that move was acknowledged
}

//api function
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string Bot::ApiCall()
{
	CURL* curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
	{
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, "https://icanhazdadjoke.com/");
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: text/plain");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
			return ("cURL request failed: " + std::string(curl_easy_strerror(res)));
        else
			return (response);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}
