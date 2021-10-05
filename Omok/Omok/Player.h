#pragma once
#include "Omok.h"

class Player {
public:
	Player(Turn turn_) : turn_(turn_) {}
	virtual ~Player() = default;

	virtual Move GetNextMove(const Omok& omok) = 0;

protected:
	Turn turn_;
};
