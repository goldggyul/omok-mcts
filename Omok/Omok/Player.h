#pragma once
#include "Board.h"

using uint = unsigned int;

class Player {
public:
	Player(Turn turn) : turn_(turn) {}
	virtual Move GetNextMove(Board& game_board) = 0;
protected:
	Turn turn_;
};
