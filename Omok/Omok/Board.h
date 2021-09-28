#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include "Exception.h"

using uint = unsigned int;

enum class Turn {
	None,
	Black,
	White
};

struct Move {
	Move() = default;
	Move(Turn turn, uint x, uint y) :turn(turn), x(x), y(y) {}
	Turn turn;
	uint x;
	uint y;
};

class Board {
public:
	Board() :size_(0), move_count_(0), board_(nullptr), result_(Turn::None) {}
	Board(const Board& other) :size_(other.size_), move_count_(other.move_count_) {
		board_ = GetBoardArray();
		for (uint i = 0; i < size_; i++) {
			for (uint j = 0; j < size_; j++) {
				board_[i][j] = other.board_[i][j];
			}
		}
	}
	~Board() {
		if (board_ != nullptr) {
			for (uint i = 0; i < size_; i++) {
				delete[] board_[i];
			}
			delete[] board_;
		}
	}

	void SetSize(uint size);
	Turn** GetBoardArray();
	bool IsGameOver();
	void Print() const;
	void PutNextMove(const Move& next_move);

	uint GetSize() const {
		return size_;
	}

	uint GetMoveCount() const {
		return move_count_;
	}


	bool IsValid(uint x, uint y) const {
		return x < size_&& y < size_;
	}

	bool IsEmpty(uint x, uint y) const {
		return board_[x][y] == Turn::None;
	}

	Turn GetResult() const {
		return result_;
	}

private:
	const std::string turn_characters_[3] = { " ","○", "●" };

	bool IsRightCompleted(Move cur_move);
	bool IsDownCompleted(Move cur_move);
	bool IsDiagonalCompleted(Move cur_move);
	bool IsCompleted(Move cur_move, const Move& dm, uint count);

	uint size_;
	uint move_count_;
	Turn** board_;
	Turn result_;
};