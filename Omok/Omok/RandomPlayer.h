#pragma once
#include "Player.h"
#include <random>

class RandomPlayer : public Player {
public:
	RandomPlayer(Turn turn_) :Player(turn_) {}
	virtual Move GetNextMove(const Board& game_board);
};

