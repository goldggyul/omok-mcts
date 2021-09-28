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
	Omok() {
		players_[0] = nullptr;
		players_[1] = nullptr;
		players_[2] = nullptr;

		InitPlayer();
	}

	// for test
	Omok(Player* p1, Player* p2) {
		players_[0] = nullptr;
		players_[1] = p1;
		players_[2] = p2;
	}

	~Omok() {
		if (players_[static_cast<uint>(Turn::Black)] != nullptr) {
			delete players_[static_cast<uint>(Turn::Black)];
		}
		if (players_[static_cast<uint>(Turn::White)] != nullptr) {
			delete players_[static_cast<uint>(Turn::White)];
		}
	}

	void Play();
	Turn GetResult() const;
	void PrintResult() const;

private:
	const std::string turn_names_[3] = { "", "black", "white" };

	void InitPlayer();
	void Initialize();

	// 2 player지만, 구현 상의 편의를 위해 size 3
	Player* players_[3];
	Board game_board_;
};

