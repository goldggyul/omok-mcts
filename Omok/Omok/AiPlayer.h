#pragma once
#include <thread>
#include "Player.h"
#include "MonteCarloTree.h"

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn, double exploration_parameter)
		:Player(turn), exploration_parameter_(exploration_parameter) {}
	Move GetFirstMove(uint size) const;
	virtual Move GetNextMove(const Omok& omok);

private:
	MonteCarloTree* GetPartialTree(const Omok& omok, uint max_depth, double exploration_parameter);
	// UCT 계산에 쓰이는 상수
	double exploration_parameter_;
};

