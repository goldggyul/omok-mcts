#include <iostream>
#include <iomanip>
#include "Board.h"

void Board::SetSize(uint size) {
	size_ = size;
	left_count_ = size * size;
	SetBoard();
}

void Board::SetBoard() {
	board_ = new Turn* [size_];
	for (uint i = 0; i < size_; i++) {
		// 0(Empty)으로 초기화
		board_[i] = new Turn[size_]();
	}
}

void Board::Print() {
	std::cout << std::endl;
	std::cout.setf(std::ios::left);

	std::cout << "   ";
	for (uint i = 0; i < size_; i++) {
		std::cout << std::setw(3) << i;
	}
	std::cout << std::endl;

	for (uint i = 0; i < size_; i++) {
		std::cout << std::setw(3) << i;
		for (uint j = 0; j < size_; j++) {
			std::cout << std::setw(3) << turn_characters_[static_cast<uint>(board_[i][j])];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Board::PutNextMove(const Move& next_move)
{
	board_[next_move.x][next_move.y] = next_move.turn;
}

bool Board::IsGameOver(Turn turn) {
	// →

	// ↘

	// ↓

	return false;
}
