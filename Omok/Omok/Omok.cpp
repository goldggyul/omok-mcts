#include "Omok.h"

void Omok::Play() {
	Initialize();

	// AiPlayer가 black인 경우 중앙/혹은 랜덤으로 first move 후에 진행

	Turn turn_ = Turn::White;
	// 이전 턴의 플레이어가 게임을 끝냈는지 확인
	while (!game_board_.IsGameOver(turn_)) {
		game_board_.Print();
		// 다음 플레이어로 턴을 넘김
		turn_ = (turn_ == Turn::Black) ? Turn::White : Turn::Black;
		Move next_move = players_[static_cast<uint>(turn_)]->GetNextMove(game_board_);
		std::cout << turn_names_[static_cast<uint>(next_move.turn)] << ' ' << next_move.x << ' ' << next_move.y << std::endl;
		game_board_.PutNextMove(next_move);
	}
	PrintResult();
}

void Omok::Initialize() {
	uint size;
	// std::cin >> size;
	size = 5;
	game_board_.SetSize(size);

	// std::string user_turn;
	// std::cin >> user_turn;

	// 1P: user, 2P: user
	//players_[static_cast<uint>(Turn::Black)] = new UserPlayer(Turn::Black);
	//players_[static_cast<uint>(Turn::White)] = new UserPlayer(Turn::White);

	// 1P: random, 2P: random
	//players_[static_cast<uint>(Turn::Black)] = new RandomPlayer(Turn::Black);
	//players_[static_cast<uint>(Turn::White)] = new RandomPlayer(Turn::White);

	// 1P: random, 2P: AI
	players_[static_cast<uint>(Turn::Black)] = new RandomPlayer(Turn::Black);
	players_[static_cast<uint>(Turn::White)] = new AiPlayer(Turn::White);
}

void Omok::PrintResult() {
	game_board_.Print();
	
	Turn result = game_board_.GetResult();
	if (result == Turn::None) {
		std::cout << "Draw - Game board is full" << std::endl;
	} else {
		std::cout << turn_names_[static_cast<uint>(result)] << " " << "win!" << std::endl;
	}
}
