#pragma once
#include <vector>
#include <queue>
#include <random>
#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include "Omok.h"

// for debugging
#include <fstream>

class Score {
public:
	Score() {}
	Score(int none_, int black_, int white_) :none_(none_), black_(black_), white_(white_) {}
	Score(const Score& other) {
		none_ = other.none_;
		black_ = other.black_;
		white_ = other.white_;
	}
	Score(Turn turn) {
		switch (turn) {
		case Turn::Black:
			black_ = 1;
			break;
		case Turn::White:
			white_ = 1;
			break;
		default:
			none_ = 1;
			break;
		}
	}

	int GetReward(Turn turn)  const {
		switch (turn) {
		case Turn::Black:
			return black_;
		case Turn::White:
			return white_;
		default:
			return none_;
		}
	}

	int GetVisitCnt() const {
		return none_ + black_ + white_;
	}

	Score& operator+= (const Score& other) {
		none_ += other.none_;
		black_ += other.black_;
		white_ += other.white_;
		return *this;
	}
private:
	int none_ = 0;
	int black_ = 0;
	int white_ = 0;
};

class MonteCarloTree {
public:
	MonteCarloTree(const Omok& game_board, Turn ai_turn, double exploration_parameter) :ai_turn_(ai_turn), exploration_parameter_(exploration_parameter), rollout_cnt_(0) {
		root_ = new MonteCarloNode(game_board, Move{ ai_turn_,0,0 }, exploration_parameter);
	}
	~MonteCarloTree() {
		//std::thread t(&MonteCarloTree::MonteCarloNode::FreeTreeNode, root_);
		//t.detach();
		root_->FreeTreeNode();
	}

	void AddNodesUntilMaxDepth(uint max_depth);
	Move GetMctsBestMove();
	void InitialRollout();
	void Print();

private:
	class MonteCarloNode {
	public:
		MonteCarloNode(const Omok& omok, Move move, double exploration_parameter)
			:omok_(omok), move_(move), reward_sum_(0), visit_cnt(0), parent_(nullptr), exploration_parameter_(exploration_parameter) {}
		MonteCarloNode(const Omok& omok, Move move, double exploration_parameter, MonteCarloNode* parent)
			:MonteCarloNode(omok, move, exploration_parameter)
		{
			parent_ = parent;
			omok_.PutNextMove(move);
		}

		void FreeTreeNode();
		void RecursiveFreeNode();
		void AddChildren();
		std::vector<Move> GetPossibleMoves(const Omok& board, Turn turn);
		// MCTS
		Score RecursiveRollout();
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

	// Recursive: 각 노드마다 child를 더해줌
	void RecursiveAddNodesUntilMaxDepth(MonteCarloNode* node, uint cur_depth, uint max_depth);
	void PrintRootAndChildrenMapAndUct(MonteCarloNode* best_node);

	MonteCarloNode* root_;
	Turn ai_turn_;
	double exploration_parameter_;
	uint rollout_cnt_;
};