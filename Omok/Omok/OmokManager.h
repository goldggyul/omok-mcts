﻿#pragma once
#include "UserPlayer.h"
#include "AiPlayer.h"

class OmokManager {
public:
	OmokManager(uint size, Turn ai_turn) {
		players_[0] = nullptr;
		if (ai_turn == Turn::Black) {
			players_[1] = new AiPlayer(Turn::Black, sqrt(2));
			players_[2] = new UserPlayer(Turn::White);
		} else {
			players_[1] = new UserPlayer(Turn::Black);
			players_[2] = new AiPlayer(Turn::White, sqrt(2));
		}
		omok_.InitGameBoard(size);
	}
	~OmokManager() {
		delete players_[1];
		delete players_[2];
	}

	void Play();
	Turn GetResult() const;
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

