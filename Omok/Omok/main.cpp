#include <iostream>
#include "OmokManager.h"

int main() {
	uint size=12;
	Turn ai_turn = Turn::Black;

	std::string user_input;
	std::cout << "size? ";
	std::cin >> size;
	std::cout << "AI turn? ";
	std::cin >> user_input;

	if (user_input == "black") {
		ai_turn = Turn::Black;
	} else if (user_input == "white"){
		ai_turn = Turn::White;
	}

	OmokManager omok_manager(size, ai_turn);
	omok_manager.Play();

	Turn turn = omok_manager.GetResult();
	if (turn == ai_turn) {
		std::cout << "win" << std::endl;
	}
	else if (turn == Turn::None) {
		std::cout << "draw" << std::endl;
	}
	else {
		std::cout << "lose" << std::endl;
	}

	return 0;
}