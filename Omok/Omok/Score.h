#pragma once

class Score {
public:
	Score() {}
	Score(int none, int black, int white) :none_(none), black_(black), white_(white) {}
	Score(const Score& other) {
		none_ = other.none_;
		black_ = other.black_;
		white_ = other.white_;
	}
	Score(Turn turn) {
		switch (turn) {
		case Turn::Black:
			black_ = 1;
			break;
		case Turn::White:
			white_ = 1;
			break;
		default:
			none_ = 1;
			break;
		}
	}

	int GetReward(Turn turn)  const {
		switch (turn) {
		case Turn::Black:
			return black_;
		case Turn::White:
			return white_;
		default:
			return none_;
		}
	}

	int GetVisitCnt() const {
		return none_ + black_ + white_;
	}

	Score& operator+= (const Score& other) {
		none_ += other.none_;
		black_ += other.black_;
		white_ += other.white_;
		return *this;
	}

private:
	int none_ = 0;
	int black_ = 0;
	int white_ = 0;
};
