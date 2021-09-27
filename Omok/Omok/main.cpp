#include <iostream>
#include "Omok.h"

int main() {

	Omok omok;
	try {
		omok.Play();
		omok.PrintResult(); // 후에 출력 함수 분리
		/*
		if (omok.IsUserWin()) {
			std::cout << win << std::endl;
		} else {
			std::cout << lose << std::endl;
		}
		*/
	} catch (...) {
		// TODO
	}
	return 0;
}