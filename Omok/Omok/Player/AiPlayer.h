#pragma once
#include <thread>
#include "Player.h"
#include "../MonteCarlo/Tree.h"

// for debugging
#include <fstream>

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn, double exploration_parameter)
		:Player(turn), exploration_parameter_(exploration_parameter) {}
	Move GetFirstMove(uint size) const;
	virtual Move GetNextMove(const Omok& omok);
	Move SearchTree(const Omok& omok);
	
private:
	Tree* GetPartialTree(const Omok& omok, uint max_depth, double exploration_parameter);
	void SearchEachTree(std::vector<Tree*>* trees);
	void DeleteTrees(std::vector<Tree*>* trees);

	// UCT 계산에 쓰이는 상수
	double exploration_parameter_;
};

