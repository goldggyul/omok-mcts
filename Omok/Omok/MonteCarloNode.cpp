#include "MonteCarloNode.h"

void MonteCarloNode::FreeTreeNode()
{
	RecursiveFreeNode();
	delete this;
}

void MonteCarloNode::RecursiveFreeNode() {
	for (auto* child : children_) {
		if (child->IsLeafNode()) {
			delete child;
		}
		else {
			child->RecursiveFreeNode();
			delete child;
		}
	}
}

void MonteCarloNode::AddChildren() {
	// 현재 게임이 종료되었으므로 child를 더하는 것이 의미가 없음
	if (IsGameOver()) {
		return;
	}
	Turn next_turn;
	// 현재 root 노드인 경우
	if (parent_ == nullptr) {
		next_turn = move_.turn;
	}
	else {
		next_turn = (move_.turn == Turn::Black) ? Turn::White : Turn::Black;
	}
	std::vector<Move> possible_moves = GetPossibleMoves(omok_, next_turn);
	for (const Move& move : possible_moves) {
		children_.push_back(new MonteCarloNode(omok_, move, exploration_parameter_, this));
	}
}

std::vector<Move> MonteCarloNode::GetPossibleMoves(const Omok& board, Turn turn)
{
	std::vector<Move> possible_moves;

	uint size = board.GetSize();
	bool** visited = new bool* [size];
	for (uint i = 0; i < size; i++) {
		visited[i] = new bool[size]();
	}

	// 오른쪽 부터 시계방향으로 
	int dx[8] = { 0,1,1,1,0,-1,-1,-1 };
	int dy[8] = { 1,1,0,-1,-1,-1,0,1 };
	for (uint i = 0; i < size; i++) {
		for (uint j = 0; j < size; j++) {
			if (!board.IsEmpty(i, j)) {
				// 8방향 살펴봄
				for (uint d = 0; d < 8; d++) {
					uint nx = i + dx[d];
					uint ny = j + dy[d];
					if (nx < size && ny < size && board.IsEmpty(nx, ny) && !visited[nx][ny]) {
						visited[nx][ny] = true;
						possible_moves.push_back(Move(turn, nx, ny));
					}
				}
			}
		}
	}
	return possible_moves;
}


Score MonteCarloNode::RolloutLeafChild() {
	Score total_score;
	std::vector<std::future<Score>> futures;
	for (MonteCarloNode* node : children_) {
		if (node->IsLeafNode()) {
			futures.push_back(std::async(&MonteCarloNode::Rollout, node));
		}
		else {
			total_score += node->RolloutLeafChild();
		}
	}
	for (auto& e : futures) {
		total_score += e.get();
	}
	// 자신의 값 업데이트
	UpdateScore(total_score);
	return total_score;
}

Score MonteCarloNode::RandomRollout() {
	if (children_.size() == 0) {
		return Score();
	}

	std::vector<uint> random_idx(children_.size());
	for (uint i = 0; i < children_.size(); i++) {
		random_idx[i] = i;
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(random_idx.begin(), random_idx.end(), gen);

	std::vector<std::future<Score>> futures;
	// 랜덤으로 child 선택: 4개 선택
	for (uint i = 0; i < 4; i++) {
		futures.push_back(std::async(&MonteCarloNode::Rollout, children_.at(i)));
	}
	Score total_score;
	for (auto& e : futures) {
		total_score += e.get();
	}
	// 자신의 값 업데이트
	UpdateScore(total_score);
	return total_score;
}

Score MonteCarloNode::Rollout() {
	Omok omok = omok_;
	Turn turn = move_.turn;

	while (!omok.IsGameOver()) {
		turn = (turn == Turn::Black) ? Turn::White : Turn::Black;
		std::vector<Move> possible_moves = GetPossibleMoves(omok, turn);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(0, possible_moves.size() - 1);

		uint index = dis(gen);
		Move next_move = possible_moves.at(index);
		omok.PutNextMove(next_move);
	}
	Score result(omok.GetResult());
	// 자신의 값 업데이트
	UpdateScore(result);
	return result;
}

void MonteCarloNode::UpdateScore(const Score& score)
{
	int reward = score.GetReward(move_.turn);
	reward_sum_ += reward;
	visit_cnt += score.GetVisitCnt();
}

void MonteCarloNode::Backpropagation(const Score& score) {
	// 루트까지 UCT 업데이트
	MonteCarloNode* cur = this->parent_;
	while (cur != nullptr) {
		cur->UpdateScore(score);
		cur = cur->parent_;
	}
}

MonteCarloNode* MonteCarloNode::ChoseChildByUct() {
	double best_uct = 0.0;
	MonteCarloNode* best_children = nullptr;

	for (MonteCarloNode* child : children_) {
		if (child->visit_cnt == 0) {
			return child;
		}
		double uct = child->CalculateUct();
		if (best_children == nullptr || uct > best_uct) {
			best_uct = uct;
			best_children = child;
		}
	}
	return best_children;
}

double MonteCarloNode::CalculateUct() const {
	double reward_mean = (double)reward_sum_ / visit_cnt;
	double exploration_term = sqrt(log(parent_->visit_cnt) / visit_cnt);
	return reward_mean + exploration_parameter_ * exploration_term;
}

MonteCarloNode* MonteCarloNode::ChoseBestChild() {
	double best_eval = 0.0;
	MonteCarloNode* best_children = nullptr;

	// for debugging
	uint sum = 0;

	for (MonteCarloNode* child : children_) {
		sum += child->GetVisitCnt();
		double eval = child->CalculateEvaluation();
		if (best_children == nullptr || eval > best_eval) {
			best_eval = eval;
			best_children = child;
		}
	}
	std::cout << "child visit 총 합: " << sum << std::endl;
	return best_children;
}

double MonteCarloNode::CalculateEvaluation() const
{
	return (double)reward_sum_ / visit_cnt;
}


void MonteCarloNode::PrintBoard() const {
	omok_.Print();
}

void MonteCarloNode::PrintInfo(std::ofstream& fout) const
{
	//std::cout << std::setw(16) << parent_->visit_cnt << "|" << std::setw(17) << visit_cnt << "|" << std::setw(16) << reward_sum_ << "|" << std::setw(16) << CalculateUct() << "|" << std::endl;
	fout << std::setw(16) << parent_->visit_cnt << "|" << std::setw(17) << visit_cnt << "|" << std::setw(16) << reward_sum_ << "|" << std::setw(16) << CalculateEvaluation() << "|" << std::endl;
}

std::vector<MonteCarloNode*>& MonteCarloNode::GetChildren() {
	return children_;
}

bool MonteCarloNode::IsGameOver() {
	return omok_.IsGameOver();
}

Move MonteCarloNode::GetMove() const {
	return move_;
}

bool MonteCarloNode::IsLeafNode() const {
	return children_.empty();
}

bool MonteCarloNode::IsFirstVisit() const {
	return visit_cnt == 0;
}

MonteCarloNode* MonteCarloNode::GetParent() {
	return parent_;;
}

Turn MonteCarloNode::GetTurn() const {
	return move_.turn;
}