#include "UserPlayer.h"

Move UserPlayer::GetNextMove(const Omok& omok)
{
    uint x, y;
    std::cin >> x >> y;
    while (!omok.IsValid(x, y)||!omok.IsEmpty(x,y)) {
        std::cout << "wrong input. input again"<<std::endl;
        std::cin.clear();
        std::cin.ignore(LLONG_MAX, '\n');
        std::cin >> x >> y;
    }
    return 	Move(turn_, x, y);
}
