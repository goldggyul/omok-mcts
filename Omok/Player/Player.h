#pragma once
#include "../Game/Omok.h"

class Player {
public:
	Player(Turn turn) : turn_(turn) {}
	virtual ~Player() = default;

	virtual Move GetNextMove(const Omok& omok) = 0;

protected:
	Turn turn_;
};
