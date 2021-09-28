#include "AiPlayer.h"

Move AiPlayer::GetFirstMove(const Board& game_board)
{
    return Move(turn_, game_board.GetSize()/2, game_board.GetSize() / 2);
}

Move AiPlayer::GetNextMove(const Board& game_board)
{
    if (game_board.GetMoveCount() == 0) {
        return GetFirstMove(game_board);
    }

    MonteCarloTree* tree = GetPartialTree(game_board, 2, exploration_parameter_);
    Move next_move = tree->GetMctsBestMove();
    delete tree;
    return next_move;
}

MonteCarloTree* AiPlayer::GetPartialTree(const Board& game_board, uint max_depth, double exploration_parameter)
{
    MonteCarloTree* tree = new MonteCarloTree(game_board, turn_, exploration_parameter);
    tree->AddNodesUntilMaxDepth(max_depth);
    return tree;
}
