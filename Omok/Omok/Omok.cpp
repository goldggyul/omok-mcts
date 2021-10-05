#include "Omok.h"

void Omok::InitGameBoard(uint size) {
	size_ = size;
	board_ = GetBoardArray();
}

Turn** Omok::GetBoardArray() {
	Turn** arr = new Turn * [size_];
	for (uint i = 0; i < size_; i++) {
		arr[i] = new Turn[size_]();
	}
	return arr;
}

bool Omok::IsGameOver() {
	return IsGameOver(Turn::Black, 5) || IsGameOver(Turn::White, 5);
}

bool Omok::IsGameOver(Turn turn, uint max_cnt) {
	for (uint i = 0; i < size_; i++) {
		for (uint j = 0; j < size_; j++) {
			Move move(turn, i, j);
			// 각 칸마다 오른쪽 방향/아래 방향/오른쪽 아래 대각선 방향 확인
			if (IsRightCompleted(move, max_cnt) || IsDownCompleted(move, max_cnt) || IsDownDiagonalCompleted(move, max_cnt) || IsUpDiagonalCompleted(move, max_cnt)) {
				result_ = turn;
				return true;
			}
		}
	}

	if (GetMoveCount() == size_ * size_) {
		result_ = Turn::None;
		return true;
	}
	return false;
}

void Omok::PutNextMove(const Move& next_move) {
	move_count_++;
	board_[next_move.x][next_move.y] = next_move.turn;
}

bool Omok::IsRightCompleted(Move cur_move, uint max_cnt) const {
	// dm: 변화량. 우측 확인하므로 dx==0, dy==1
	Move dm(cur_move.turn, 0, 1);
	return IsCompleted(cur_move, dm, 0, max_cnt);
}

bool Omok::IsDownCompleted(Move cur_move, uint max_cnt) const {
	Move dm(cur_move.turn, 1, 0);
	return IsCompleted(cur_move, dm, 0, max_cnt);
}

bool Omok::IsDownDiagonalCompleted(Move cur_move, uint max_cnt) const {
	Move dm(cur_move.turn, 1, 1);
	return IsCompleted(cur_move, dm, 0, max_cnt);
}

bool Omok::IsUpDiagonalCompleted(Move cur_move, uint max_cnt) const {
	Move dm(cur_move.turn, -1, 1);
	return IsCompleted(cur_move, dm, 0, max_cnt);
}

// Recursive: dm씩 이동하면서 5개가 완성되었는지 확인
bool Omok::IsCompleted(Move cur_move, const Move& dm, uint count, uint max_cnt) const {
	if (count == 5) {
		return true;
	}
	if (!IsValid(cur_move.x, cur_move.y)) {
		return false;
	}
	if (board_[cur_move.x][cur_move.y] == cur_move.turn) {
		cur_move.x += dm.x;
		cur_move.y += dm.y;
		return IsCompleted(cur_move, dm, count + 1, max_cnt);
	} else {
		return false;
	}
}

void Omok::PrintBoard() const {
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
			std::cout << std::setw(3) << GetTurnCharacter(board_[i][j]);
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
