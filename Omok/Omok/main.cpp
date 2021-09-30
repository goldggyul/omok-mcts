#include <iostream>
#include "OmokManager.h"

int main() {
	uint size=12;
	Turn user_turn = Turn::Black;
	std::ofstream fout("uct_info.txt");
	std::ofstream fout2("win_log.txt");

	uint black_cnt = 0, white_cnt = 0, draw_cnt = 0;
	for (uint i = 0; i < 10; i++) {
		OmokManager omok(size, new AiPlayer(Turn::White, sqrt(2)), new AiPlayer(Turn::Black, 2));
		omok.Play();
		Turn turn = omok.GetResult();
		switch (turn) {
		case Turn::Black:
			black_cnt++;
			break;
		case Turn::White:
			white_cnt++;
			break;
		case Turn::None:
			draw_cnt++;
			break;
		}
		fout2 << "black: " << black_cnt << " white: " << white_cnt << " draw: " << draw_cnt << std::endl;
	}
	fout2.close();
	fout.close();

	return 0;
}