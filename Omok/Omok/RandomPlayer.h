#pragma once
#include "Player.h"
#include <random>

class RandomPlayer : public Player {
public:
	RandomPlayer(Turn turn) :Player(turn) {}
	virtual Move GetNextMove(Board& game_board);

private:
};

