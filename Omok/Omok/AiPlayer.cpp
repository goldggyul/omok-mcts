#include "AiPlayer.h"

Move AiPlayer::GetFirstMove(uint size) {
	return Move(turn_, size / 2, size / 2);
}

Move AiPlayer::GetNextMove(const Omok& omok) {
	// AiPlayer가 black인 경우 중앙/혹은 랜덤으로 first move 후에 진행
	if (omok.GetMoveCount() == 0) {
		return GetFirstMove(omok.GetSize());
	}

	// 부분 트리 만듦 -> 이 트리를 복사해서 병렬로 트리 탐색을 진행하고 후에 결과를 합침
	MonteCarloTree* first_tree = GetPartialTree(omok, 1, exploration_parameter_);
	// 후에 다른 쓰레드가 메모리 해제하게할 것이므로 트리 벡터는 포인터로 선언해야함
	std::vector<MonteCarloTree*>* copy_trees = new std::vector<MonteCarloTree*>;
	copy_trees->push_back(first_tree);

	// 트리의 개수, 각 트리를 병렬로 탐색 후 후에 결과를 합침
	uint tree_cnt = 8;
	std::vector<std::thread> tree_workers;
	std::mutex vec_mtx;
	// 부분 트리 만든 것 복사
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
	Move next_move = first_tree->GetBestMove();

	// 트리 각 노드들 메모리 해제는 다른 쓰레드에게 맡긴 후 detach(언제 종료되든 상관X)
	// 삭제에 최대 거의 2초(약 1700ms쯤)까지 걸리기 때문 
	std::thread tree_deletion([copy_trees] {
		for (MonteCarloTree* tree : *copy_trees) {
			delete tree;
		}
		delete copy_trees;
		});
	tree_deletion.detach();

	return next_move;
}

MonteCarloTree* AiPlayer::GetPartialTree(const Omok& game_board, uint max_depth, double exploration_parameter) {
	MonteCarloTree* tree = new MonteCarloTree(game_board, turn_, exploration_parameter);
	tree->AddNodesUntilMaxDepth(max_depth);
	return tree;
}
