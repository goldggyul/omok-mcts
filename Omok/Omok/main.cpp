#include <iostream>
#include "Omok.h"

int main() {
	Omok omok(new AiPlayer(Turn::Black, 1/sqrt(2)), new UserPlayer(Turn::White));
	omok.Play();
	return 0;
}