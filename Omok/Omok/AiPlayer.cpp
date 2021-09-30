#include "AiPlayer.h"

Move AiPlayer::GetFirstMove(uint size)
{
	return Move(turn_, size / 2, size / 2);
}

Move AiPlayer::GetNextMove(const Omok& game_board)
{
	auto ai_start = std::chrono::steady_clock::now();

	std::cout << cnt_ << "번째" << std::endl;
	std::ofstream fout("uct_info.txt", std::ios::app);
	fout << cnt_ << "번째" << std::endl;
	cnt_++;

	// AiPlayer가 black인 경우 중앙/혹은 랜덤으로 first move 후에 진행
	if (game_board.GetMoveCount() == 0) {
		return GetFirstMove(game_board.GetSize());
	}

	MonteCarloTree* first_tree = GetPartialTree(game_board, 1, exploration_parameter_);
	auto tree_end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tree_end - ai_start).count();
	fout << "> 부분 트리 생성: " << elapsed << "ms 경과" << std::endl;
	std::cout << "> 부분 트리 생성: " << elapsed << "ms 경과" << std::endl;

	std::vector<MonteCarloTree*> copy_trees;
	copy_trees.push_back(first_tree);

	uint thread_cnt = 4;
	std::vector<std::thread> tree_workers;
	std::mutex vec_mtx;

	// 부분 트리 만든 것 복사, 0번 원본 트리 제외
	for (uint i = 1; i < thread_cnt; i++) {
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

	auto copy_end = std::chrono::steady_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(copy_end - tree_end).count();
	fout << "> 부분 트리 복사: " << elapsed << "ms 경과" << std::endl;
	std::cout << "> 부분 트리 복사: " << elapsed << "ms 경과" << std::endl;


	// 복사 후 각 트리에 대해 시뮬레이션하여 계산
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(copy_end - ai_start).count();
	for (auto* tree : copy_trees) {
		tree_workers.push_back(std::thread(&MonteCarloTree::Mcts, tree));
	}
	for (auto& worker : tree_workers) {
		worker.join();
	}
	auto simulation_end = std::chrono::steady_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(simulation_end - tree_end).count();
	fout << "> 트리 전체 시뮬레이션: " << elapsed << "ms 경과" << std::endl;
	std::cout << "> 트리 전체 시뮬레이션: " << elapsed << "ms 경과" << std::endl;

	// 나머지 트리들 first_tree에 merge
	first_tree->WriteRootAndChildrenInfoToFile(nullptr);
	for (uint i = 1; i < thread_cnt; i++) {
		copy_trees[i]->WriteRootAndChildrenInfoToFile(nullptr);
		first_tree->MergeTreeValues(copy_trees[i]);
	}
	first_tree->WriteRootAndChildrenInfoToFile(nullptr);

	Move next_move = first_tree->GetBestMove();
	auto next_move_end = std::chrono::steady_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(next_move_end - tree_end).count();
	fout << "> 다음 수 결정: " << elapsed << "ms 경과" << std::endl;
	std::cout << "> 다음 수 결정: " << elapsed << "ms 경과" << std::endl;

	delete first_tree;
	auto tree_delete_end = std::chrono::steady_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tree_delete_end - next_move_end).count();
	fout << "> 트리 삭제: " << elapsed << "ms 경과" << std::endl;
	std::cout << "> 트리 삭제: " << elapsed << "ms 경과" << std::endl;

	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tree_delete_end - ai_start).count();
	fout << "> 총 " << elapsed << "ms 경과" << std::endl << std::endl;
	std::cout << "> 총 " << elapsed << "ms 경과" << std::endl;
	fout.close();
	return next_move;
}

MonteCarloTree* AiPlayer::GetPartialTree(const Omok& game_board, uint max_depth, double exploration_parameter)
{
	MonteCarloTree* tree = new MonteCarloTree(game_board, turn_, exploration_parameter);
	tree->AddNodesUntilMaxDepth(max_depth);
	return tree;
}
