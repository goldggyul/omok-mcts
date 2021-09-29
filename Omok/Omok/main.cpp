#include <iostream>
#include "OmokManager.h"

int main() {
	uint size=12;
	Turn user_turn = Turn::Black;
	// OmokManager omok_(size, user_turn);
	OmokManager omok(size, new AiPlayer(Turn::Black, sqrt(2)), new RandomPlayer(Turn::White));
	omok.Play();
	return 0;
}