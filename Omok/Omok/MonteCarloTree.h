#pragma once
#include "MonteCarloNode.h"

class MonteCarloTree {
public:
	MonteCarloTree(const Omok& omok, Turn ai_turn, double exploration_parameter)
		:ai_turn_(ai_turn), exploration_parameter_(exploration_parameter) {
		root_ = new MonteCarloNode(omok, Move{ ai_turn_,0,0 }, exploration_parameter, nullptr);
	}
	// 시뮬레이션 시작 전, 부분 트리 복사하여 병렬 실행 위한 복사 생성자
	MonteCarloTree(const MonteCarloTree& other)
		:ai_turn_(other.ai_turn_), exploration_parameter_(other.exploration_parameter_) {
		// 부분 트리 복사
		root_ = other.root_->MakeCopyOfTree();
	}
	~MonteCarloTree() {
		root_->FreeTreeNode();
	}

	void AddNodesUntilMaxDepth(uint max_depth);
	void Mcts();
	Move GetBestMove();
	void MergeTreeValues(MonteCarloTree* other);

private:
	// Recursive: 각 노드마다 child를 더해줌
	void RecursiveAddNodesUntilMaxDepth(MonteCarloNode* node, uint cur_depth, uint max_depth);

	MonteCarloNode* root_;
	Turn ai_turn_;
	double exploration_parameter_;
};