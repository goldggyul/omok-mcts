#pragma once
// for print tree
#include "Player.h"
#include "MonteCarloTree.h"

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn_) :Player(turn_) {}
	Move GetFirstMove(const Board& game_board);
	virtual Move GetNextMove(const Board& game_board);

private:
	MonteCarloTree* GetPartialTree(const Board& game_board, uint max_depth);
};

