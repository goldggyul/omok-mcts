#include "AiPlayer.h"

Move AiPlayer::GetFirstMove(const Omok& game_board)
{
	return Move(turn_, game_board.GetSize() / 2, game_board.GetSize() / 2);
}

Move AiPlayer::GetNextMove(const Omok& game_board)
{
	auto start = std::chrono::steady_clock::now();

	std::cout << cnt_ << "번째" << std::endl;
	std::ofstream fout("uct_info.txt", std::ios::app);
	fout << cnt_ << "번째" << std::endl;

	cnt_++;
	// AiPlayer가 black인 경우 중앙/혹은 랜덤으로 first move 후에 진행
	if (game_board.GetMoveCount() == 0) {
		return GetFirstMove(game_board);
	}

	MonteCarloTree* tree = GetPartialTree(game_board, 2, exploration_parameter_);
	Move next_move = tree->GetMctsBestMove();
	delete tree;

	auto end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	fout << "총" << elapsed << "ms 경과" << std::endl << std::endl;
	fout.close();
	return next_move;
}

MonteCarloTree* AiPlayer::GetPartialTree(const Omok& game_board, uint max_depth, double exploration_parameter)
{
	MonteCarloTree* tree = new MonteCarloTree(game_board, turn_, exploration_parameter);
	tree->AddNodesUntilMaxDepth(max_depth);
	return tree;
}
