#include <iostream>
#include "Omok.h"

int main() {

	Omok omok;
	try {
		omok.Play();
		// 후에 출력 함수 분리 ??
		omok.PrintResult(); 
		/*
		if (omok.IsUserWin()) {
			std::cout << win << std::endl;
		} else {
			std::cout << lose << std::endl;
		}
		*/
	}
	catch (...) {
		// TODO
	}
	return 0;
}