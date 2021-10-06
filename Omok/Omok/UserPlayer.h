#pragma once
#include "Player.h"

class UserPlayer : public Player {
public:
	UserPlayer(Turn turn) :Player(turn) {}
	virtual Move GetNextMove(const Omok& omok);
};

