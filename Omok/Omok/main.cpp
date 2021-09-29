#include <iostream>
#include "OmokManager.h"

int main() {
	uint size=12;
	Turn user_turn = Turn::Black;
	std::ofstream fout("uct_info.txt");
	std::ofstream fout2("win_log.txt");
	// OmokManager omok_(size, user_turn);
	for (uint i = 0; i < 9; i++) {
		OmokManager omok(size, new AiPlayer(Turn::White, sqrt(2)), new AiPlayer(Turn::Black, 2));
		omok.Play();
	}
	fout.close();
	fout2.close();
	return 0;
}