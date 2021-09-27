#include "Omok.h"

void Omok::Play() {
	Initialize();

	Turn turn = Turn::White;
	// ���� ���� �÷��̾ ������ ���´��� Ȯ��
	while (!game_board_.IsGameOver(turn)) {
		game_board_.Print();
		// ���� �÷��̾�� ���� �ѱ�
		turn = (turn == Turn::Black) ? Turn::White : Turn::Black;
		Move next_move = players_[static_cast<uint>(turn)]->GetNextMove(game_board_);
		PrintMove(next_move);
		game_board_.PutNextMove(next_move);
	}
}

void Omok::Initialize() {
	uint size;
	// std::cin >> size;
	size = 5;
	game_board_.SetSize(size);

	// std::string user_turn;
	// std::cin >> user_turn;

	//players_[static_cast<uint>(Turn::Black)] = new UserPlayer(Turn::Black);
	//players_[static_cast<uint>(Turn::White)] = new UserPlayer(Turn::White);

	players_[static_cast<uint>(Turn::Black)] = new RandomPlayer(Turn::Black);
	players_[static_cast<uint>(Turn::White)] = new RandomPlayer(Turn::White);
}

void Omok::PrintResult() {
}
