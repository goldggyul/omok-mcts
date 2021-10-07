#include "OmokManager.h"

void OmokManager::Play() {
	Turn turn_ = Turn::White;
	while (!omok_.IsGameOver()) {
		omok_.PrintBoard();

		// 다음 플레이어의 다음 수
		turn_ = (turn_ == Turn::Black) ? Turn::White : Turn::Black;
		Move next_move = GetPlayer(turn_)->GetNextMove(omok_);
		omok_.PutNextMove(next_move);

		std::cout << omok_.GetTurnName(next_move.turn) << ' ' << next_move.x << ' ' << next_move.y << std::endl;
	}
	omok_.PrintBoard();
}

Turn OmokManager::GetResult() const {
	return omok_.GetResult();
}
