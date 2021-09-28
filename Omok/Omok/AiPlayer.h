#pragma once
#include "Player.h"
#include "MonteCarloTree.h"

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn_, double exploration_parameter) :Player(turn_), exploration_parameter_(exploration_parameter){}
	Move GetFirstMove(const Board& game_board);
	virtual Move GetNextMove(const Board& game_board);

private:
	MonteCarloTree* GetPartialTree(const Board& game_board, uint max_depth, double exploration_parameter);
	double exploration_parameter_;
};

