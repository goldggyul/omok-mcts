#include "AiPlayer.h"

Move AiPlayer::GetFirstMove(const Board& game_board)
{
    return Move();
}

Move AiPlayer::GetNextMove(const Board& game_board)
{
    MonteCarloTree* tree = GetPartialTree(game_board, 2);
    tree->Print();
    return Move();

    //Move next_move = tree->GetMctsResult();
    //delete tree;
    //return next_move;
}

MonteCarloTree* AiPlayer::GetPartialTree(const Board& game_board, uint max_depth)
{
    MonteCarloTree* tree = new MonteCarloTree(game_board, turn_);
    tree->AddNodesUntilMaxDepth(max_depth);
    return tree;
}
