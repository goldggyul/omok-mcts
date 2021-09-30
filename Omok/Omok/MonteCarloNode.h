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

	void SetParent(MonteCarloNode* parent);
	MonteCarloNode* MakeCopyOfTree();
	void MakeCopyOfChildrenToOther(MonteCarloNode* parent);
	void FreeTreeNode();
	void RecursiveFreeNode();
	void AddChildren();
	std::vector<Move> GetPossibleMoves(const Omok& board, Turn turn);
	// MCTS
	Score RolloutLeafChild();
	Score RandomRollout();
	Score Rollout();
	void UpdateScore(const Score& score);
	void Backpropagation(const Score& score);
	MonteCarloNode* ChoseChildByUct();
	double CalculateUct() const;
	MonteCarloNode* ChoseBestChild();
	double CalculateEvaluation() const;
	
	void MergeRootNode(MonteCarloNode* other);

	bool IsGameOver();
	void PrintBoard() const;
	void PrintInfo(std::ofstream& fout) const;
	void PrintInfo() const;
	std::vector<MonteCarloNode*>& GetChildren();
	void PushToChildren(MonteCarloNode* child) {
		children_.push_back(child);
	}
	Move GetMove() const;
	bool IsLeafNode() const;
	bool IsFirstVisit() const;
	uint GetVisitCnt() const {
		return visit_cnt_;
	}
	MonteCarloNode* GetParent();

private:
	Turn GetTurn() const;
	Omok omok_;
	Move move_;
	int reward_sum_;
	uint visit_cnt_;
	MonteCarloNode* parent_;
	std::vector<MonteCarloNode*> children_;
	double exploration_parameter_;
};