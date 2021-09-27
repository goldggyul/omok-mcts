#pragma once
#include <iostream>
#include <string>
#include "Player.h"
#include "UserPlayer.h"
#include "AiPlayer.h"
#include "RandomPlayer.h"
#include "Board.h"

class Omok {
public:
	~Omok() {
		if (players_[static_cast<uint>(Turn::Black)] != nullptr) {
			delete players_[static_cast<uint>(Turn::Black)];
		}
		if (players_[static_cast<uint>(Turn::White)] != nullptr) {
			delete players_[static_cast<uint>(Turn::White)];
		}
	}

	void Play();
	// size, turn 입력받음
	void Initialize();
	void PrintResult();

private:
	const std::string turn_names_[3] = { "", "black", "white" };

	// 2 player지만, 구현 상의 편의를 위해 size 3
	Player* players_[3];
	Board game_board_;
};

