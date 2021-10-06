#pragma once
#include <string>
#include <iostream>
#include <iomanip>

//for debugging
#include <fstream>

using uint = unsigned int;

enum class Turn {
	None,
	Black,
	White
};

struct Move {
	Move(Turn turn, uint x, uint y) :turn(turn), x(x), y(y) {}
	Turn turn;
	uint x;
	uint y;
};

class Omok {
public:
	Omok() :size_(0), move_count_(0), board_(nullptr), result_(Turn::None) {}
	Omok(const Omok& other) :size_(other.size_), move_count_(other.move_count_) {
		board_ = GetBoardArray();
		for (uint i = 0; i < size_; i++) {
			for (uint j = 0; j < size_; j++) {
				board_[i][j] = other.board_[i][j];
			}
		}
	}
	~Omok() {
		if (board_ != nullptr) {
			for (uint i = 0; i < size_; i++) {
				delete[] board_[i];
			}
			delete[] board_;
		}
	}
	void InitGameBoard(uint size);
	void PutNextMove(const Move& next_move);

	// 어느 차례든 게임이 끝나는지
	bool IsGameOver();

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

	void PrintBoard() const;
	const std::string GetTurnCharacter(Turn turn) const {
		switch (turn) {
		case Turn::Black:
			return "●";
		case Turn::White:
			return "○";
		default:
			return " ";
		}
	}
	const std::string GetTurnName(Turn turn) const {
		switch (turn) {
		case Turn::Black:
			return "black";
		case Turn::White:
			return "white";
		default:
			return "none";
		}
	}

	//for debugging
	void PrintBoard(std::ofstream& fout) const;

private:
	Turn** GetBoardArray();

	bool IsGameOver(Turn turn, uint max_cnt);
	bool IsRightCompleted(Move cur_move, uint max_cnt) const;
	bool IsDownCompleted(Move cur_move, uint max_cnt) const;
	bool IsDownDiagonalCompleted(Move cur_move, uint max_cnt) const;
	bool IsUpDiagonalCompleted(Move cur_move, uint max_cnt) const;
	bool IsCompleted(Move cur_move, const Move& dm, uint count, uint max_cnt) const;

	uint size_;
	uint move_count_;
	Turn** board_;
	Turn result_;
};