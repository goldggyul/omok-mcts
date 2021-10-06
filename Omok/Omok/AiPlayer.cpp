#include "AiPlayer.h"

// 첫 수를 둘 경우만 따로 처리
Move AiPlayer::GetFirstMove(uint size) const {
	return Move(turn_, size / 2, size / 2);
}

Move AiPlayer::GetNextMove(const Omok& omok) {
	if (omok.GetMoveCount() == 0) {
		return GetFirstMove(omok.GetSize());
	}	

	// 부분 트리 만듦 -> 이 트리를 복사해서 병렬로 트리 탐색을 진행하고 후에 결과를 합침
	MonteCarloTree* first_tree = GetPartialTree(omok, 1, exploration_parameter_);
	// 후에 다른 쓰레드가 메모리 해제하게할 것이므로 트리 벡터는 포인터로 선언해야함
	std::vector<MonteCarloTree*>* copy_trees = new std::vector<MonteCarloTree*>;
	copy_trees->push_back(first_tree);

	// 트리의 개수, 각 트리를 병렬로 탐색 후에 결과를 합침
	uint tree_cnt = 11;
	std::vector<std::thread> tree_workers;
	std::mutex vec_mtx;
	// 부분 트리 만든 것 병렬로 복사
	for (uint i = 1; i < tree_cnt; i++) {
		tree_workers.push_back(std::thread([first_tree, &copy_trees, &vec_mtx] {
			MonteCarloTree* tree = new MonteCarloTree(*first_tree);
			vec_mtx.lock();
			copy_trees->push_back(tree);
			vec_mtx.unlock();
			}));
	}
	for (auto& worker : tree_workers) {
		worker.join();
	}
	tree_workers.clear();

	// Root parallelization: thread 이용하여 병렬로 트리 탐색 진행
	for (auto* tree : *copy_trees) {
		tree_workers.push_back(std::thread(&MonteCarloTree::Mcts, tree));
	}
	for (auto& worker : tree_workers) {
		worker.join();
	}
	tree_workers.clear();

	// 나머지 트리들의 점수를 첫번째 트리에 합침
	for (uint i = 1; i < tree_cnt; i++) {
		first_tree->MergeTreeValues(copy_trees->at(i));
	}

	// Voting Algorithm: 가장 많은 표를 받은 수를 best로 선택
	std::vector<uint> tree_votes;
	for (auto tree : *copy_trees) {
		tree_votes.push_back(tree->GetBestChildIndex());
	}
	Move next_move = first_tree->GetMostVotedMove(tree_votes);

	// 트리 각 노드들 메모리 해제는 다른 쓰레드에게 맡긴 후 detach(언제 종료되든 상관없으므로)
	// 삭제에 몇 초가 걸리기 때문
	std::thread tree_deletion([copy_trees] {
		for (MonteCarloTree* tree : *copy_trees) {
			delete tree;
		}
		delete copy_trees;
		});
	tree_deletion.detach();

	return next_move;
}

// 인자인 최대 깊이까지 부분 트리를 생성
MonteCarloTree* AiPlayer::GetPartialTree(const Omok& game_board, uint max_depth, double exploration_parameter) {
	// 시뮬레이션 시작 시 최소 depth 1의 부분 트리 필요
	if (max_depth < 1) {
		max_depth = 1;
	}
	MonteCarloTree* tree = new MonteCarloTree(game_board, turn_, exploration_parameter);
	tree->AddNodesUntilMaxDepth(max_depth);
	return tree;
}
