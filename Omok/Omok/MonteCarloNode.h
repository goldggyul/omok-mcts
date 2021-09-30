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
	// 노드가 트리 루트일때
	MonteCarloNode(const Omok& omok, Move move, double exploration_parameter)
		:omok_(omok), move_(move), reward_sum_(0), visit_cnt(0), parent_(nullptr), exploration_parameter_(exploration_parameter) {}
	// 노드가 트리 루트가 아닐 때 -> Parent와 연결
	MonteCarloNode(const Omok& omok, Move move, double exploration_parameter, MonteCarloNode* parent)
		:MonteCarloNode(omok, move, exploration_parameter)
	{
		parent_ = parent;
		omok_.PutNextMove(move);
	}
	MonteCarloNode(const MonteCarloNode& other)
		:MonteCarloNode(other.omok_, other.move_, exploration_parameter_)
	{

	}

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

	bool IsGameOver();
	void PrintBoard() const;
	void PrintInfo(std::ofstream& fout) const;
	std::vector<MonteCarloNode*>& GetChildren();
	Move GetMove() const;
	bool IsLeafNode() const;
	bool IsFirstVisit() const;
	uint GetVisitCnt() const {
		return visit_cnt;
	}
	MonteCarloNode* GetParent();

private:
	Turn GetTurn() const;
	Omok omok_;
	Move move_;
	int reward_sum_;
	uint visit_cnt;
	MonteCarloNode* parent_;
	std::vector<MonteCarloNode*> children_;
	double exploration_parameter_;
};