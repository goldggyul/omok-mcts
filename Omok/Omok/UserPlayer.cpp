#include "UserPlayer.h"

Move UserPlayer::GetNextMove(const Omok& game_board)
{
    uint x, y;
    std::cin >> x >> y;
    while (!game_board.IsValid(x, y)||!game_board.IsEmpty(x,y)) {
        std::cout << "wrong input. input again"<<std::endl;
        std::cin >> x >> y;
    }
    return 	Move(turn_, x, y);
}
