#pragma once
#include <string>

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

	uint GetTurn() {
		return static_cast<uint>(turn);
	}

};

class Board {
public:
	void SetSize(uint size);
	bool IsGameOver(Turn turn);
	void Print();
	void PutNextMove(const Move& next_move);

	uint GetSize() const {
		return size_;
	}

	bool IsValid(uint x, uint y) const {
		return 0 <= x && x < size_ && 0 <= y && y <= size_;
	}

	bool IsEmpty(uint x, uint y) const {
		return board_[x][y] == Turn::None;
	}

private:
	void SetBoard();

	std::string turn_characters_[3] = {" ","¡Ü", "¡Û"};
	uint size_;
	uint left_count_;
	Turn** board_;
};
