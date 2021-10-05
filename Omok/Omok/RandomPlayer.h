#pragma once
#include <random>
#include "Player.h"

class RandomPlayer : public Player {
public:
	RandomPlayer(Turn turn_) :Player(turn_) {}
	virtual Move GetNextMove(const Omok& omok);
};

