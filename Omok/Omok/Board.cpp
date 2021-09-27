#include <iostream>
#include <iomanip>
#include "Board.h"

void Board::SetSize(uint size) {
	size_ = size;
	empty_count_ = size * size;

	board_ = new Turn* [size_];
	for (uint i = 0; i < size_; i++) {
		board_[i] = new Turn[size_]();
	}
}

bool Board::IsGameOver(Turn turn) {
	for (uint i = 0; i < size_; i++) {
		for (uint j = 0; j < size_; j++) {
			Move move{ turn, i, j };
			// 각 칸마다 오른쪽 방향/아래 방향/오른쪽 아래 대각선 방향 확인
			if (IsRightCompleted(move) || IsDownCompleted(move) || IsDiagonalCompleted(move)) {
				result_ = turn;
				return true;
			}
		}
	}
	if (GetEmptyCount() == 0) {
		result_ = Turn::None;
		return true;
	}
	return false;
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

void Board::PutNextMove(const Move& next_move) {
	empty_count_--;
	board_[next_move.x][next_move.y] = next_move.turn;
}

bool Board::IsRightCompleted(Move cur_move) {
	// dm: 변화량. 우측 확인하므로 dx==0, dy==1
	Move dm{ cur_move.turn,0,1 };
	return IsCompleted(cur_move, dm, 0);
}

bool Board::IsDownCompleted(Move cur_move) {
	Move dm{ cur_move.turn,1,0 };
	return IsCompleted(cur_move, dm, 0);
}

bool Board::IsDiagonalCompleted(Move cur_move) {
	Move dm{ cur_move.turn,1,1 };
	return IsCompleted(cur_move, dm, 0);
}

// Recursive: dm씩 이동하면서 5개가 완성되었는지 확인
bool Board::IsCompleted(Move cur_move, const Move& dm, uint count) {
	if (count == 5) {
		return true;
	}
	if (!IsValid(cur_move.x, cur_move.y)) {
		return false;
	}
	if (board_[cur_move.x][cur_move.y] == cur_move.turn) {
		cur_move.x += dm.x;
		cur_move.y += dm.y;
		return IsCompleted(cur_move, dm, count + 1);
	}
	else {
		return false;
	}
}
