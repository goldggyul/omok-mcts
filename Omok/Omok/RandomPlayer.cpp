#include "RandomPlayer.h"

Move RandomPlayer::GetNextMove(Board& game_board)
{
	// �õ尪�� ��� ���� random_device ����
	std::random_device rd;
	// random_device �� ���� ���� ���� ������ �ʱ�ȭ
	std::mt19937 gen(rd());
	// 0 ���� size ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����
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
