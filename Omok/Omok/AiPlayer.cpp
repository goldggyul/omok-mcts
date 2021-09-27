#include "AiPlayer.h"

Move AiPlayer::GetFirstMove(const Board& game_board)
{
    return Move();
}

Move AiPlayer::GetNextMove(const Board& game_board)
{
    Tree* tree = GetPartialTree(game_board, 2);
    //tree->Print();

    return Move();
}

AiPlayer::Tree* AiPlayer::GetPartialTree(const Board& game_board, uint max_depth)
{
    Turn prior_turn = (turn_ == Turn::Black) ? Turn::White : Turn::Black;
    Tree* tree = new Tree(new Node(game_board, prior_turn));
    tree->AddNodesUntilMaxDepth(max_depth);
    return tree;
}
