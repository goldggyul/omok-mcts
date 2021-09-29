#pragma once
#include <vector>
#include <queue>
#include <random>
#include <cmath>
#include "Omok.h"

// for debugging
#include <fstream>
#include <chrono>

class MonteCarloTree {
public:
	MonteCarloTree(const Omok& game_board, Turn ai_turn, double exploration_parameter) :ai_turn_(ai_turn), exploration_parameter_(exploration_parameter){
		root_ = new MonteCarloNode(game_board, Move{ ai_turn_,0,0}, exploration_parameter);
	}
	~MonteCarloTree() {
		// TODO: Node들 메모리 해제
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

		void AddChildren();
		std::vector<Move> GetPossibleMoves(const Omok& board, Turn turn);
		// MCTS
		void RecursiveRollout();
		void Rollout();
		void Backpropagation(Turn winner);
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
};

