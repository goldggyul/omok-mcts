#pragma once
#include <vector>
#include <queue>
#include <random>
#include <cmath>
#include "Board.h"

class MonteCarloTree {
public:
	MonteCarloTree(const Board& game_board, Turn ai_turn, double exploration_parameter) :ai_turn_(ai_turn), exploration_parameter_(exploration_parameter){
		root_ = new MonteCarloNode(game_board, Move{ ai_turn_,0,0}, exploration_parameter);
	}
	~MonteCarloTree() {
		// TODO: Node들 메모리 해제
	}

	void AddNodesUntilMaxDepth(uint max_depth);
	void Print();
	Move GetMctsBestMove();
	void InitialRollout();

private:
	class MonteCarloNode {
	public:
		MonteCarloNode(const Board& board, Move move, double exploration_parameter)
			:board_(board), move_(move), reward_sum_(0), visit_cnt(0), parent_(nullptr), exploration_parameter_(exploration_parameter) {}
		MonteCarloNode(const Board& board, Move move, double exploration_parameter, MonteCarloNode* parent)
			:MonteCarloNode(board, move, exploration_parameter)
		{
			parent_ = parent;
			board_.PutNextMove(move);
		}

		void AddChildren();
		std::vector<Move> GetPossibleMoves(const Board& board, Turn turn);
		void PrintBoard() const;
		std::vector<MonteCarloNode*>& GetChildren();

		bool IsGameOver();
		Move GetMove() const;

		// MCTS
		bool IsLeafNode() const;
		bool IsFirstVisit() const;
		void RecursiveRollout();
		void Rollout();
		void Backpropagation(Turn winner);
		MonteCarloNode* GetParent();
		MonteCarloNode* ChoseChildByUct();

	private:
		Turn GetTurn() const;
		double CalculateUct(uint np, uint nj, int reward_sum);

		Board board_;
		Move move_;
		int reward_sum_;
		uint visit_cnt;
		MonteCarloNode* parent_;
		std::vector<MonteCarloNode*> children_;
		double exploration_parameter_;
	};

	// Recursive: 각 노드마다 child를 더해줌
	void RecursiveAddNodesUntilMaxDepth(MonteCarloNode& node, uint cur_depth, uint max_depth);

	MonteCarloNode* root_;
	Turn ai_turn_;
	double exploration_parameter_;
};

