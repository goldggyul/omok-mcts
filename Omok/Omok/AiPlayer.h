#pragma once
#include <thread>
#include "Player.h"
#include "MonteCarloTree.h"

// for debugging
#include <fstream>

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn_, double exploration_parameter)
		:Player(turn_), exploration_parameter_(exploration_parameter) {
		// for debugging
		std::ofstream fout("mcts_info.txt");
		fout.close();
	}
	Move GetFirstMove(uint size);
	virtual Move GetNextMove(const Omok& omok);
	Move GetNextMoveWithoutPrint(const Omok& omok);
	Move GetNextMoveWithPrint(const Omok& omok);

private:
	MonteCarloTree* GetPartialTree(const Omok& omok, uint max_depth, double exploration_parameter);
	double exploration_parameter_;
};

