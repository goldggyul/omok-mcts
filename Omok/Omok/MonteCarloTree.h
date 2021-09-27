#pragma once
#include <vector>
#include <queue>
#include "Board.h"

class MonteCarloTree {
public:
	MonteCarloTree(const Board& game_board, Turn ai_turn) :ai_turn_(ai_turn) {
		root_ = new MonteCarloNode(game_board, Move{ ai_turn_,0,0 });
	}
	~MonteCarloTree() {
		// TODO: Node들 메모리 해제
	}

	void AddNodesUntilMaxDepth(uint max_depth);
	void Print();
	Move GetMctsResult();

private:
	class MonteCarloNode {
	public:
		MonteCarloNode(const Board& board, Move move)
			:board_(board), move_(move), mean_reward_(0), parent_visit_cnt(0), visit_cnt(0), parent_(nullptr) {}
		MonteCarloNode(const Board& board, Move move, MonteCarloNode* parent)
			:MonteCarloNode(board, move)
		{
			parent_ = parent;
			board_.PutNextMove(move);
		}
		void AddChild();
		bool IsLeafNode();
		bool IsFirstVisit();
		void Rollout();
		void Backpropagation();
		MonteCarloNode* ChoseChildrenByUcb();
		void PrintBoard();
		std::vector<MonteCarloNode*>& GetChild();
		
	private:
		Board board_;
		Move move_;
		uint mean_reward_;
		uint parent_visit_cnt;
		uint visit_cnt;
		MonteCarloNode* parent_;
		std::vector<MonteCarloNode*> child_;
	};

	// Recursive: 각 노드마다 child를 더해줌
	void RecursiveAddNodesUntilMaxDepth(MonteCarloNode& node, uint cur_depth, uint max_depth);

	MonteCarloNode* root_;
	Turn ai_turn_;
};

