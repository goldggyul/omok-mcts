﻿#include "MonteCarloTree.h"

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
		if (root_->IsEnoughSearch()) {
			break;
		}
		if (cur_elapsed > 10000 - init_elapsed - 30) { // 10000ms(10초) 제한
			break;
		}
		if (cur_node->IsLeafNode()) {
			Score score;
			if (cur_node->IsFirstVisit()) {
				score = cur_node->Rollout();
			} else {
				cur_node->AddChildren();
				if (cur_node->IsLeafNode()) {
					score = cur_node->Rollout();
				} else {
					// 지정 개수의 child만 랜덤으로 골라서 rollout
					int child_cnt = 4;
					score = cur_node->RandomRollout(child_cnt);
				}
			}
			cur_node->Backpropagation(score);
			cur_node = root_;
		} else {
			cur_node = cur_node->SelectChildByUct();
		}
	}
}

Move MonteCarloTree::GetBestMove() {
	return root_->SelectBestMove();
}

void MonteCarloTree::MergeTreeValues(MonteCarloTree* other) {
	root_->MergeRootAndChild(other->root_);
}

void MonteCarloTree::PrintInfo(std::ofstream& fout) const {
	fout << "------------------------------------------------------" << std::endl;
	fout << "|  no. | 부모 방문 횟수 |  내 방문 횟수 | reward sum |" << std::endl;
	fout << "------------------------------------------------------" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "|  no. | 부모 방문 횟수 |  내 방문 횟수 | reward sum |" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl;

	uint cnt = 1;
	for (const auto* child : root_->GetChildren()) {
		fout << "|" << std::setw(6) << cnt;
		std::cout << "|" << std::setw(6) << cnt;
		cnt++;
		fout << "|";
		std::cout << "|";
		child->PrintInfo(fout);
	}
	fout << "------------------------------------------------------" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl;
}
