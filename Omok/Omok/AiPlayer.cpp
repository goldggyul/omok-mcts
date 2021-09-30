#include "AiPlayer.h"

Move AiPlayer::GetFirstMove(uint size)
{
	return Move(turn_, size / 2, size / 2);
}

Move AiPlayer::GetNextMove(const Omok& game_board)
{
	auto ai_start = std::chrono::steady_clock::now();

	// AiPlayer가 black인 경우 중앙/혹은 랜덤으로 first move 후에 진행
	if (game_board.GetMoveCount() == 0) {
		return GetFirstMove(game_board.GetSize());
	}

	// 부분 트리 만듦 -> 이 트리를 복사해서 병렬로 트리 탐색을 진행하고 후에 결과를 합친다.
	MonteCarloTree* first_tree = GetPartialTree(game_board, 1, exploration_parameter_);
	std::vector<MonteCarloTree*> copy_trees;
	copy_trees.push_back(first_tree);

	// 트리 탐색을 진행할 thread 개수
	uint mcts_cnt = 4;
	std::vector<std::thread> tree_workers;
	std::mutex vec_mtx;
	// 부분 트리 만든 것 복사
	for (uint i = 1; i < mcts_cnt; i++) {
		tree_workers.push_back(std::thread([first_tree, &copy_trees, &vec_mtx] {
			MonteCarloTree* tree = new MonteCarloTree(*first_tree);
			vec_mtx.lock();
			copy_trees.push_back(tree);
			vec_mtx.unlock();
			}));
	}
	for (auto& worker : tree_workers) {
		worker.join();
	}
	tree_workers.clear();

	// Root parallelization: thread 이용하여 병렬로 트리 탐색 진행
	for (auto* tree : copy_trees) {
		tree_workers.push_back(std::thread(&MonteCarloTree::Mcts, tree));
	}
	for (auto& worker : tree_workers) {
		worker.join();
	}

	// 나머지 트리들의 점수를 첫번째 트리에 합침
	for (uint i = 1; i < mcts_cnt; i++) {
		first_tree->MergeTreeValues(copy_trees[i]);
	}
	Move next_move = first_tree->GetBestMove();
	delete first_tree;
	return next_move;
}

MonteCarloTree* AiPlayer::GetPartialTree(const Omok& game_board, uint max_depth, double exploration_parameter) {
	MonteCarloTree* tree = new MonteCarloTree(game_board, turn_, exploration_parameter);
	tree->AddNodesUntilMaxDepth(max_depth);
	return tree;
}
