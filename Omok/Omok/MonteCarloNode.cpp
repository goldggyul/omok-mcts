#include "MonteCarloNode.h"

// 루트 노드에서 호출
// 자식 노드들 메모리 해제하고 마지막으로 자신 해제
void MonteCarloNode::FreeTreeNode() {
	RecursiveFreeNode();
	delete this;
}
void MonteCarloNode::RecursiveFreeNode() {
	for (auto* child : children_) {
		if (child->IsLeafNode()) {
			delete child;
		} else {
			child->RecursiveFreeNode();
			delete child;
		}
	}
}

// 루트 노드에서 호출, MCTS 시작 전 부분 트리를 만듦
MonteCarloNode* MonteCarloNode::MakeCopyOfTree() {
	MonteCarloNode* copied_root = new MonteCarloNode(omok_, move_, exploration_parameter_, nullptr);
	CopyChildrenToOtherNode(copied_root);
	return copied_root;
}
void MonteCarloNode::CopyChildrenToOtherNode(MonteCarloNode* parent) {
	if (IsLeafNode()) {
		return;
	}
	for (auto* child : children_) {
		MonteCarloNode* copy = new MonteCarloNode(*child);
		copy->SetParent(parent);
		parent->PushToChildren(copy);
		child->CopyChildrenToOtherNode(copy);
	}
}

// 모든 Rollout 함수는 마지막에 자신의 score만 업데이트하고 종료
// 즉 BackPropagtion이 필요하다면 따로 호출
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

Score MonteCarloNode::RolloutLeafChild() {
	Score total_score;

	// future이용, 병렬로 모든 LeafChild를 rollout 후에 결과를 모아서 score 업데이트
	std::vector<std::future<Score>> futures;
	for (MonteCarloNode* node : children_) {
		if (node->IsLeafNode()) {
			futures.push_back(std::async(&MonteCarloNode::Rollout, node));
		} else {
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
		return Rollout();
	} 
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, children_.size() - 1);
	uint index = dis(gen);
	return children_[index]->Rollout();
}

bool MonteCarloNode::IsGameOver() {
	return omok_.IsGameOver();
}

bool MonteCarloNode::IsEnoughSearch() const {
	uint max_visit = 15000, min_visit = 200;

	if (visit_cnt_ > max_visit) {
		return true;
	} else if (visit_cnt_ < min_visit) {
		return false;
	}

	// 방문 횟수 1등과 2등이 최소 방문수 이상 차이나면 충분히 탐색한 것으로 판단
	MonteCarloNode* first_child = nullptr;
	MonteCarloNode* second_child = nullptr;
	for (MonteCarloNode* child : children_) {
		if (first_child == nullptr || child->visit_cnt_ > first_child->visit_cnt_) {
			second_child = first_child;
			first_child = child;
		} else if (second_child == nullptr || child->visit_cnt_ > second_child->visit_cnt_) {
			second_child = child;
		}
	}
	if (!first_child || !second_child) {
		return false;
	} else if (first_child->visit_cnt_ > second_child->visit_cnt_ + min_visit) {
		//std::cout << "최대 방문: " << first_child->visit_cnt_ << " / 다음 방문: " << second_child->visit_cnt_ << " / 차이 "<< min_visit <<" 이상!" << std::endl;
		return true;
	}

	return false;
}

void MonteCarloNode::UpdateScore(const Score& score) {
	uint reward = score.GetReward(move_.turn);
	reward_sum_ += reward;
	visit_cnt_ += score.GetVisitCnt();
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
	} else {
		next_turn = (move_.turn == Turn::Black) ? Turn::White : Turn::Black;
	}
	std::vector<Move> possible_moves = GetPossibleMoves(omok_, next_turn);
	for (const Move& move : possible_moves) {
		MonteCarloNode* child = new MonteCarloNode(omok_, move, exploration_parameter_, this);
		child->omok_.PutNextMove(move);
		children_.push_back(child);
	}
}

// 다른 수가 놓여져있지 않다면 무조건 리턴. 즉, 게임 종료 여부를 체크하지 않음
std::vector<Move> MonteCarloNode::GetPossibleMoves(const Omok& omok, Turn turn) {
	std::vector<Move> possible_moves;

	uint size = omok.GetSize();
	bool** visited = new bool* [size];
	for (uint i = 0; i < size; i++) {
		visited[i] = new bool[size]();
	}

	// 한칸 주변
	// 오른쪽 부터 시계방향으로 
	int dx[8] = { 0,1,1,1,0,-1,-1,-1 };
	int dy[8] = { 1,1,0,-1,-1,-1,0,1 };
	for (uint i = 0; i < size; i++) {
		for (uint j = 0; j < size; j++) {
			if (!omok.IsEmpty(i, j)) {
				// 8방향 살펴봄
				for (uint d = 0; d < 8; d++) {
					uint nx = i + dx[d];
					uint ny = j + dy[d];
					if (nx < size && ny < size && omok.IsEmpty(nx, ny) && !visited[nx][ny]) {
						visited[nx][ny] = true;
						possible_moves.push_back(Move(turn, nx, ny));
					}
				}
			}
		}
	}
	for (uint i = 0; i < size; i++) {
		delete[] visited[i];
	}
	delete[] visited;

	return possible_moves;
}

void MonteCarloNode::Backpropagation(const Score& score) {
	// 루트까지 score 업데이트, 맨 처음 호출한 노드는 제외
	MonteCarloNode* cur = this->parent_;
	while (cur != nullptr) {
		cur->UpdateScore(score);
		cur = cur->parent_;
	}
}

MonteCarloNode* MonteCarloNode::SelectChildByUct() {
	double best_uct = 0.0;
	MonteCarloNode* best_children = nullptr;

	for (MonteCarloNode* child : children_) {
		if (child->visit_cnt_ == 0) {
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
	double reward_mean = (double)reward_sum_ / visit_cnt_;
	double exploration_term = sqrt(log(parent_->visit_cnt_) / visit_cnt_);
	return reward_mean + exploration_parameter_ * exploration_term;
}

void MonteCarloNode::MergeRootAndChild(MonteCarloNode* other) {
	reward_sum_ += other->reward_sum_;
	visit_cnt_ += other->visit_cnt_;
	for (uint i = 0; i < children_.size(); i++) {
		children_[i]->reward_sum_ += other->reward_sum_;
		children_[i]->visit_cnt_ += other->visit_cnt_;
	}
}

uint MonteCarloNode::SelectBestChild() const {
	uint best_index = 0;
	uint best_eval = children_[best_index]->CalculateEvaluation();

	for (uint i = 1; i < children_.size(); i++) {
		uint eval = children_[i]->CalculateEvaluation();
		if (eval > best_eval) {
			best_index = i;
			best_eval = eval;
		}
	}
	return best_index;
}

uint MonteCarloNode::CalculateEvaluation() const {
	return visit_cnt_;
}

Move MonteCarloNode::GetChildMove(uint index) const {
	return children_[index]->move_;
}

Move MonteCarloNode::GetMostVotedMove(const std::vector<uint>& votes) const {
	// 투표 받은 횟수 세기
	std::vector<uint> counter(children_.size(), 0);
	for (uint e : votes) {
		counter[e]++;
	}
	// 가장 많이 뽑힌 인덱스 구하기 
	uint most_frequent_idx = 0;
	uint visit = children_[0]->visit_cnt_;
	for (uint i = 0; i < counter.size(); i++) {
		if (counter[i] > counter[most_frequent_idx]) {
			most_frequent_idx = i;
		} else if (counter[i] == counter[most_frequent_idx]) {
			if (children_[i]->visit_cnt_ > visit) {
				visit = children_[i]->visit_cnt_;
				most_frequent_idx = i;
			}
		}
	}
	return children_[most_frequent_idx]->move_;
}

// for debugging
void MonteCarloNode::PrintInfo(std::ofstream& fout) const {
	fout << std::setw(16) << parent_->visit_cnt_ << "|" << std::setw(15) << visit_cnt_ << "|" << std::setw(12) << reward_sum_ << "|" << std::endl;
	//std::cout << std::setw(16) << parent_->visit_cnt_ << "|" << std::setw(15) << visit_cnt_ << "|" << std::setw(12) << reward_sum_ << "|" << std::endl;
}

void MonteCarloNode::PrintBoard(std::ofstream& fout) const {
	omok_.PrintBoard(fout);
}
