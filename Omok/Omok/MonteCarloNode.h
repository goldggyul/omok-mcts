#pragma once
#include <vector>
#include <queue>
#include <random>
#include <cmath>
#include <chrono>
#include <future>
#include <algorithm>
#include "Omok.h"
#include "Score.h"

// for debugging
#include <fstream>

class MonteCarloNode {
public:
	MonteCarloNode(const Omok& omok, Move move, double exploration_parameter, MonteCarloNode* parent)
		:omok_(omok), move_(move), reward_sum_(0), visit_cnt_(0), parent_(parent), exploration_parameter_(exploration_parameter) {}
	MonteCarloNode(const MonteCarloNode& other)
		:MonteCarloNode(other.omok_, other.move_, other.exploration_parameter_, nullptr) {}
	void FreeTreeNode();
	void RecursiveFreeNode();

	MonteCarloNode* MakeCopyOfTree();
	void CopyChildrenToOtherNode(MonteCarloNode* parent);

	void SetParent(MonteCarloNode* parent) {
		parent_ = parent;
	}
	std::vector<MonteCarloNode*>& GetChildren() {
		return children_;
	}
	void PushToChildren(MonteCarloNode* child) {
		children_.push_back(child);
	}

	// MCTS
	bool IsLeafNode() const {
		return children_.empty();
	}
	bool IsFirstVisit() const {
		return visit_cnt_ == 0;
	}
	Score Rollout();
	Score RolloutLeafChild();
	Score RandomRollout(uint cnt);
	bool IsGameOver();
	bool IsEnoughSearch() const;
	void UpdateScore(const Score& score);
	void AddChildren();
	std::vector<Move> GetPossibleMoves(const Omok& board, Turn turn);
	void Backpropagation(const Score& score);

	MonteCarloNode* SelectChildByUct();
	double CalculateUct() const;
	void MergeRootAndChild(MonteCarloNode* other);
	Move SelectBestMove() const;
	uint CalculateEvaluation() const;

	// for debugging
	void PrintInfo(std::ofstream& fout) const;

private:
	Omok omok_;
	Move move_;
	uint reward_sum_;
	uint visit_cnt_;
	MonteCarloNode* parent_;
	std::vector<MonteCarloNode*> children_;
	double exploration_parameter_;
};