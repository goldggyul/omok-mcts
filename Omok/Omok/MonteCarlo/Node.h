#pragma once
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <future>
#include "../Game/Omok.h"
#include "Score.h"

// 충분히 탐색했는가 판단 기준
const uint MaxVisit = 20000;
const uint MinVisit = 150;

class Node {
public:
	Node(const Omok& omok, Move move, double exploration_parameter, Node* parent)
		:omok_(omok), move_(move), reward_sum_(0), visit_cnt_(0), parent_(parent), exploration_parameter_(exploration_parameter) {}
	// 새로운 노드를 만들 때 사용되는 복사 생성자
	Node(const Node& other)
		:Node(other.omok_, other.move_, other.exploration_parameter_, nullptr) {}
	void FreeTreeNode();
	void RecursiveFreeNode();

	Node* GetCopyOfTree() const;
	void CopyChildrenToOtherNode(Node* parent) const;
	void RecursiveAddNodesUntilMaxDepth(uint cur_depth, uint max_depth);

	// MCTS
	bool IsLeafNode() const {
		return children_.empty();
	}
	bool IsFirstVisit() const {
		return visit_cnt_ == 0;
	}
	Score Rollout();
	Score RolloutLeafChild();
	Score RandomRollout();
	bool IsGameOver();
	bool IsEnoughSearch() const;
	void UpdateScore(const Score& score);
	void AddChildren();
	std::vector<Move> GetPossibleMoves(const Omok& omok, Turn turn) const;
	void Backpropagation(const Score& score);
	Node* SelectChildByUct();
	double CalculateUct() const;
	void MergeChildrenValues(Node* other);
	uint SelectBestChild() const;
	uint CalculateEvaluation() const;
	Move GetMostVotedMove(const std::vector<uint>& votes) const;
	
private:
	Omok omok_;
	Move move_;
	uint reward_sum_;
	uint visit_cnt_;
	Node* parent_;
	std::vector<Node*> children_;
	double exploration_parameter_;
};