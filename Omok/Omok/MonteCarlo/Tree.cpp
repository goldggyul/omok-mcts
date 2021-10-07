#include "Tree.h"

void Tree::AddNodesUntilMaxDepth(uint max_depth) {
	root_->RecursiveAddNodesUntilMaxDepth(0, max_depth);
}

std::vector<Tree*>* Tree::GetCopies(uint tree_cnt) {
	std::vector<Tree*>* copy_trees = new std::vector<Tree*>;
	uint cnt = 0;

	// 원본 트리 가장 앞에 포함
	copy_trees->push_back(this);
	cnt++;
	
	std::vector<std::thread> workers;
	std::mutex vec_mtx;
	while (cnt < tree_cnt) {
		workers.push_back(std::thread([this, copy_trees, &vec_mtx] {
			Tree* tree = new Tree(*this);
			vec_mtx.lock();
			copy_trees->push_back(tree);
			vec_mtx.unlock();
			}));
		cnt++;
	}
	for (auto& worker : workers) {
		worker.join();
	}
	return copy_trees;
}

void Tree::Mcts() {
	auto start = std::chrono::steady_clock::now();
	// 한 번씩 child 모두 rollout
	root_->RolloutLeafChild();
	auto init_end = std::chrono::steady_clock::now();
	auto init_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(init_end - start).count();

	Node* cur_node = root_;
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

uint Tree::GetBestMoveIndex() const {
	return root_->SelectBestChild();
}

void Tree::MergeTreesValues(std::vector<Tree*>* trees) {
	// 나머지 트리들의 점수를 트리에 합침
	for (const auto& tree : *trees) {
		if (tree == this) {
			continue;
		}
		root_->MergeChildrenValues(tree->root_);
	}
}

Move Tree::GetMostVotedMove(const std::vector<uint>& votes) const {
	return root_->GetMostVotedMove(votes);
}
