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

	MonteCarloTree* tree = GetPartialTree(game_board, 2, exploration_parameter_);
	auto tree_end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tree_end - ai_start).count();
	fout << "> 부분 트리 생성: " << elapsed << "ms 경과" << std::endl;
	std::cout << "> 부분 트리 생성: " << elapsed << "ms 경과" << std::endl;

	std::vector<MonteCarloTree*> trees;
	trees.push_back(tree);
	// 세 개 복사
	for (uint i = 0; i < 3; i++) {
		trees.push_back(new MonteCarloTree(*tree));
	}
	// 복사 후 바로 GetMctsBestMove

	// Merge tree


	Move next_move = tree->GetMctsBestMove();
	auto next_move_end = std::chrono::steady_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(next_move_end - tree_end).count();
	fout << "> 다음 수 결정: " << elapsed << "ms 경과" << std::endl;
	std::cout << "> 다음 수 결정: " << elapsed << "ms 경과" << std::endl;

	delete tree;
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
