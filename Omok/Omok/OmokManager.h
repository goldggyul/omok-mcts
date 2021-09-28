#pragma once
#include <iostream>
#include <string>
#include "Player.h"
#include "UserPlayer.h"
#include "AiPlayer.h"
#include "RandomPlayer.h"
#include "Omok.h"

class OmokManager {
public:
	OmokManager(uint size, Turn turn) {
		players_[0] = nullptr;
		if (turn == Turn::Black) {
			players_[1] = new UserPlayer(Turn::Black);
			players_[2] = new AiPlayer(Turn::White, sqrt(2));
		}
		else {
			players_[1] = new AiPlayer(Turn::White, sqrt(2));
			players_[2] = new UserPlayer(Turn::White);
		}
		omok_.SetSize(size);
	}

	// for test
	OmokManager(uint size, Player* p1, Player* p2) {
		players_[0] = nullptr;
		players_[1] = p1;
		players_[2] = p2;
		omok_.SetSize(size);
	}

	~OmokManager() {
		delete players_[1];
		delete players_[2];
	}

	void Play();
	Turn GetResult() const;
	void PrintResult() const;

	Player* GetPlayer(Turn turn) {
		switch (turn) {
		case Turn::Black:
			return players_[1];
		case Turn::White:
			return players_[2];
		default:
			return nullptr;
		}
	}
private:
	// 2 player지만, 구현 상의 편의를 위해 size 3
	Player* players_[3];
	Omok omok_;
};

