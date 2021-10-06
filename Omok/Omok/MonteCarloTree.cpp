#include "MonteCarloTree.h"

void MonteCarloTree::AddNodesUntilMaxDepth(uint max_depth) {
	RecursiveAddNodesUntilMaxDepth(root_, 0, max_depth);
}

// Recursive: 각 노드마다 children를 더해줌
void MonteCarloTree::RecursiveAddNodesUntilMaxDepth(MonteCarloNode* node, uint cur_depth, uint max_depth) {
	if (node->IsGameOver()) {
		return;
	}
	if (cur_depth == max_depth) {
		return;
	}
	node->AddChildren();
	for (MonteCarloNode* child : node->GetChildren()) {
		RecursiveAddNodesUntilMaxDepth(child, cur_depth + 1, max_depth);
	}
}

void MonteCarloTree::Mcts() {
	auto start = std::chrono::steady_clock::now();
	// 한 번씩 child 모두 rollout
	root_->RolloutLeafChild();
	auto init_end = std::chrono::steady_clock::now();
	auto init_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(init_end - start).count();

	MonteCarloNode* cur_node = root_;
	while (true) {
		auto end = std::chrono::steady_clock::now();
		auto cur_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - init_end).count();

		// 충분히 탐색했거나 timeout 시에 종료
		if (root_->IsEnoughSearch()) {
			break;
		}
		if (cur_elapsed > TimeLimit - init_elapsed) {
			break;
		}

		// MCTS 로직
		if (cur_node->IsLeafNode()) {
			Score score;
			if (cur_node->IsFirstVisit()) {
				score = cur_node->Rollout();
			} else {
				cur_node->AddChildren();
				score = cur_node->RandomRollout();
			}
			cur_node->Backpropagation(score);
			cur_node = root_;
		} else {
			cur_node = cur_node->SelectChildByUct();
		}
	}
}

uint MonteCarloTree::GetBestChildIndex() const {
	return root_->SelectBestChild();
}

void MonteCarloTree::MergeTreeValues(MonteCarloTree* other) {
	root_->MergeChildrenValues(other->root_);
}

Move MonteCarloTree::GetMostVotedMove(const std::vector<uint>& votes) const {
	return root_->GetMostVotedMove(votes);
}
