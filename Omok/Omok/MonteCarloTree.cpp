#include "MonteCarloTree.h"

void MonteCarloTree::AddNodesUntilMaxDepth(uint max_depth) {
	RecursiveAddNodesUntilMaxDepth(root_, 0, max_depth);
}

// Recursive: �� ��帶�� children�� ������
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
	// �� ���� child ��� rollout
	root_->RolloutLeafChild();
	auto init_end = std::chrono::steady_clock::now();
	auto init_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(init_end - start).count();

	MonteCarloNode* cur_node = root_;
	while (true) {
		auto end = std::chrono::steady_clock::now();
		auto cur_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - init_end).count();
		if (cur_elapsed > 10000 - init_elapsed - 30) { // 10000ms(10��) ����
			break;
		}
		if (cur_node->IsLeafNode()) {
			Score score;
			if (cur_node->IsFirstVisit()) {
				score = cur_node->Rollout();
			}
			else {
				cur_node->AddChildren();
				if (cur_node->IsLeafNode()) {
					score = cur_node->Rollout();
				}
				else {
					// 4���� child�� �������� ��� rollout
					score = cur_node->RandomRollout(4);
				}
			}
			cur_node->Backpropagation(score);
			cur_node = root_;
		}
		else {
			cur_node = cur_node->ChoseChildByUct();
		}
	}
}

Move MonteCarloTree::GetBestMove()
{
	return root_->ChoseBestMove();
}

void MonteCarloTree::MergeTreeValues(MonteCarloTree* other)
{
	root_->MergeRootAndChild(other->root_);
}
