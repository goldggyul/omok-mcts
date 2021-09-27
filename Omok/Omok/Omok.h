#pragma once
#include <iostream>
#include <string>
#include "Player.h"
#include "UserPlayer.h"
#include "AiPlayer.h"
#include "RandomPlayer.h"
#include "Board.h"

class Omok {
public:
	void Play();
	// size, turn 입력받음
	void Initialize(); 
	void PrintResult();

private:
	void PrintMove(Move move) {
		std::cout << turn_names_[move.GetTurn()] << ' ' << move.x << ' ' << move.y << std::endl;
	}

	// 2 player지만, 구현 상의 편의를 위해 size 3
	Player* players_[3]; 
	Board game_board_;
	const std::string turn_names_[3] = { "", "black", "white" };
};

