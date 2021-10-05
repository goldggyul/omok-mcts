#pragma once
#include <thread>
#include "Player.h"
#include "MonteCarloTree.h"

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn_, double exploration_parameter)
		:Player(turn_), exploration_parameter_(exploration_parameter) {}
	Move GetFirstMove(uint size);
	virtual Move GetNextMove(const Omok& omok);

private:
	MonteCarloTree* GetPartialTree(const Omok& omok, uint max_depth, double exploration_parameter);
	double exploration_parameter_;
};

