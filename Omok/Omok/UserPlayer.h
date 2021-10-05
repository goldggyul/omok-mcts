#pragma once
#include "Player.h"

class UserPlayer : public Player {
public:
	UserPlayer(Turn turn_) :Player(turn_) {}
	virtual Move GetNextMove(const Omok& omok);
};

