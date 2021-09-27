#pragma once
#include "Player.h"
#include <iostream>

class UserPlayer : public Player {
public:
	UserPlayer(Turn turn_) :Player(turn_) {}
	virtual Move GetNextMove(const Board& game_board);

private:
};

