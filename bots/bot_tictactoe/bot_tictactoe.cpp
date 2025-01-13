#include "bot_tictactoe.hpp"

void Bot_TicTacToe::ticTacToeReset()
{
	_board = {'1','2','3','4','5','6','7','8','9'};
	_currentPlayer = 'X';
	_gameOver = false;
	_movesCount = 0;
}

std::string Bot_TicTacToe::getBoardStr() const
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

bool Bot_TicTacToe::checkWin()
{
	static int winCombos[8][3] = {{0,1,2}, {3,4,5}, {6,7,8}, {0,3,6}, {1,4,7}, {2,5,8}, {0,4,8}, {2,4,6}};
	for (auto &combo : winCombos)
		if (_board[combo[0]] == _board[combo[1]] && _board[combo[1]] == _board[combo[2]])
			return true;
	return false;
}

void Bot_TicTacToe::TicTacToe(int pos)
{
	// if (pos < 1 || pos > 9)
	// 	//send Message to player that this move is illegal
	// if (_gameOver)
	// 	//send Message that the game is over
	// int index = pos - 1;
	// if (_board[index] == 'X' || _board[index] == 'O')
	// 	//send Message to player that this square is already taken
	// _board[index] = _currentPlayer;
	// _movesCount++;
	// if (checkWin())
	// {
	// 	_gameOver = true;
	// 	_game_is_won = true;
	// 	//send Message for both that we have a winner
	// } 
	// else if (_movesCount == 9)
	// 	_gameOver = true;
	// 	//send Message to both that it's a draw
	// _currentPlayer = (_currentPlayer == 'X') ? 'O' : 'X';
	// //send Message to both that move was acknowledged
}
