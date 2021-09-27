#pragma once
#include <string>
#include "Exception.h"

using uint = unsigned int;

enum class Turn {
	None,
	Black,
	White
};

struct Move {
	Turn turn;
	uint x;
	uint y;
};

class Board {
public:
	~Board() {
		if (board_ != nullptr) {
			for (uint i = 0; i < size_; i++) {
				delete[] board_[i];
			}
			delete[] board_;
		}
	}

	void SetSize(uint size);
	bool IsGameOver(Turn turn);
	void Print();
	void PutNextMove(const Move& next_move);

	uint GetSize() const {
		return size_;
	}

	uint GetEmptyCount() const {
		return empty_count_;
	}

	bool IsValid(uint x, uint y) const {
		return x < size_ && y < size_;
	}

	bool IsEmpty(uint x, uint y) const {
		return board_[x][y] == Turn::None;
	}

	Turn GetResult() const {
		return result_;
	}

private:
	const std::string turn_characters_[3] = { " ","●", "○" };

	bool IsRightCompleted(Move cur_move);
	bool IsDownCompleted(Move cur_move);
	bool IsDiagonalCompleted(Move cur_move);
	bool IsCompleted(Move cur_move, const Move& dm, uint count);

	uint size_;
	uint empty_count_;
	Turn** board_;
	Turn result_;
};