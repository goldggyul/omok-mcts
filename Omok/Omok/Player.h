#pragma once
#include "Omok.h"

using uint = unsigned int;

class Player {
public:
	Player(Turn turn_) : turn_(turn_) {}
	virtual ~Player() = default;

	virtual Move GetNextMove(const Omok& game_board) = 0;
	Turn GetTurn() {
		return turn_;
	}
protected:
	Turn turn_;
};
