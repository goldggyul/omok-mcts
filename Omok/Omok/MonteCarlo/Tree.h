#pragma once
#include "Node.h"

// ms 단위. 트리 탐색 제한 시간
const uint TimeLimit = 4500;

class Tree {
public:
	Tree(const Omok& omok, Turn ai_turn, double exploration_parameter)
		:ai_turn_(ai_turn), exploration_parameter_(exploration_parameter) {
		root_ = new Node(omok, Move{ ai_turn_,0,0 }, exploration_parameter, nullptr);
	}
	// 트리 탐색 시작 전, 부분 트리 복사하여 병렬 실행 위한 복사 생성자
	Tree(const Tree& other)
		:ai_turn_(other.ai_turn_), exploration_parameter_(other.exploration_parameter_) {
		// 부분 트리 복사
		root_ = other.root_->GetCopyOfTree();
	}
	~Tree() {
		root_->FreeTreeNode();
	}

	void AddNodesUntilMaxDepth(uint max_depth);
	std::vector<Tree*>* GetCopies(uint tree_cnt);
	void Mcts();
	uint GetBestMoveIndex() const;
	void MergeTreesValues(std::vector<Tree*>* trees);
	Move GetMostVotedMove(const std::vector<uint>& votes) const;

private:
	Node* root_;
	Turn ai_turn_;
	double exploration_parameter_;
};