#pragma once
#include "Board.h"

using uint = unsigned int;

class Player {
public:
	Player(Turn turn_) : turn_(turn_) {}
	virtual ~Player() = default;

	Turn GetRootTurn() {
		return turn_;
	}
	virtual Move GetNextMove(const Board& game_board) = 0;
protected:
	Turn turn_;
};
