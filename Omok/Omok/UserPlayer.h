#pragma once
#include "Player.h"
#include <iostream>

class UserPlayer : public Player {
public:
	UserPlayer(Turn turn) :Player(turn) {}
	virtual Move GetNextMove(Board& game_board);

private:
};

