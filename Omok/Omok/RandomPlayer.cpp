#include "RandomPlayer.h"

Move RandomPlayer::GetNextMove(Board& game_board)
{
	// 시드값을 얻기 위한 random_device 생성
	std::random_device rd;
	// random_device 를 통해 난수 생성 엔진을 초기화
	std::mt19937 gen(rd());
	// 0 부터 size 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의
	std::uniform_int_distribution<int> dis(0, game_board.GetSize() - 1);
	uint x = dis(gen);
	uint y = dis(gen);
	while (!game_board.IsValid(x, y)||!game_board.IsEmpty(x,y)) {
		x = dis(gen);
		y = dis(gen);
	}
	Move move;
	move.turn = turn_;
	move.x = x;
	move.y = y;
	return move;
}
