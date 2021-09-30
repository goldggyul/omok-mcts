#pragma once
#include <thread>
#include "Player.h"
#include "MonteCarloTree.h"

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn_, double exploration_parameter) :Player(turn_), exploration_parameter_(exploration_parameter), cnt_(1) {}
	Move GetFirstMove(uint size);
	virtual Move GetNextMove(const Omok& game_board);

private:
	MonteCarloTree* GetPartialTree(const Omok& game_board, uint max_depth, double exploration_parameter);
	double exploration_parameter_;
	// for debugging
	uint cnt_;
};

