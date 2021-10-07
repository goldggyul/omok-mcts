#include "Node.h"

// 루트 노드에서 호출
// 자식 노드들 메모리 해제하고 마지막으로 자신 해제
void Node::FreeTreeNode() {
	RecursiveFreeNode();
	delete this;
}
void Node::RecursiveFreeNode() {
	for (auto* child : children_) {
		if (child->IsLeafNode()) {
			delete child;
		} else {
			child->RecursiveFreeNode();
			delete child;
		}
	}
}

// 루트 노드에서 호출
// MCTS 시작 전 부분 트리를 만듦
Node* Node::MakeCopyOfTree() const {
	Node* copied_root = new Node(omok_, move_, exploration_parameter_, nullptr);
	CopyChildrenToOtherNode(copied_root);
	return copied_root;
}
void Node::CopyChildrenToOtherNode(Node* parent) const {
	for (auto* child : children_) {
		Node* copy = new Node(*child);
		copy->parent_ = parent;
		parent->children_.push_back(copy);
		child->CopyChildrenToOtherNode(copy);
	}
}

// Recursive: max depth까지 각 노드마다 children를 더해줌
void Node::RecursiveAddNodesUntilMaxDepth(uint cur_depth, uint max_depth) {
	if (IsGameOver()) {
		return;
	}
	if (cur_depth == max_depth) {
		return;
	}
	AddChildren();
	for (Node* child : children_) {
		RecursiveAddNodesUntilMaxDepth(cur_depth + 1, max_depth);
	}
}

// 모든 Rollout 함수는 마지막에 자신의 score만 업데이트하고 종료
// 즉 BackPropagtion이 필요하다면 따로 호출
Score Node::Rollout() {
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

Score Node::RolloutLeafChild() {
	Score total_score;

	// future이용, 병렬로 모든 LeafChild를 rollout 후에 결과를 모아서 score 업데이트
	std::vector<std::future<Score>> futures;
	for (Node* node : children_) {
		if (node->IsLeafNode()) {
			futures.push_back(std::async(&Node::Rollout, node));
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

// 랜덤으로 child 하나 선택하여 rollout
Score Node::RandomRollout() {
	if (children_.size() == 0) {
		return Rollout();
	} 
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, children_.size() - 1);
	uint index = dis(gen);
	return children_[index]->Rollout();
}

bool Node::IsGameOver() {
	return omok_.IsGameOver();
}

bool Node::IsEnoughSearch() const {
	if (visit_cnt_ > MaxVisit) {
		return true;
	} else if (visit_cnt_ < MinVisit) {
		return false;
	}
	// 방문 횟수 1등과 2등이 MinVisit 이상 차이나면 충분히 탐색한 것으로 판단
	Node* first_child = nullptr;
	Node* second_child = nullptr;
	for (Node* child : children_) {
		if (first_child == nullptr || child->visit_cnt_ > first_child->visit_cnt_) {
			second_child = first_child;
			first_child = child;
		} else if (second_child == nullptr || child->visit_cnt_ > second_child->visit_cnt_) {
			second_child = child;
		}
	}
	if (!first_child || !second_child) {
		return false;
	} else if (first_child->visit_cnt_ > second_child->visit_cnt_ + MinVisit) {
		return true;
	}
	return false;
}

void Node::UpdateScore(const Score& score) {
	// reward는 이겼을 시 1, 그 외 0
	uint reward = score.GetReward(move_.turn);
	reward_sum_ += reward;
	visit_cnt_ += score.GetVisitCnt();
}

void Node::AddChildren() {
	// 현재 게임이 종료되었으므로 child를 더하는 것이 의미가 없음
	if (IsGameOver()) {
		return;
	}

	Turn next_turn;
	if (parent_ == nullptr) { // 현재 root 노드인 경우는 이전 턴이 없었으므로 현재 턴이 저장되어 있음
		next_turn = move_.turn;
	} else {
		next_turn = (move_.turn == Turn::Black) ? Turn::White : Turn::Black;
	}

	std::vector<Move> possible_moves = GetPossibleMoves(omok_, next_turn);
	for (const Move& move : possible_moves) {
		Node* child = new Node(omok_, move, exploration_parameter_, this);
		child->omok_.PutNextMove(move);
		children_.push_back(child);
	}
}

// 어떤 수를 둬서 게임이 종료되는지 여부와 상관 없이 현재 놓을 수 있는 수를 모두 리턴
std::vector<Move> Node::GetPossibleMoves(const Omok& omok, Turn turn) const {
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

void Node::Backpropagation(const Score& score) {
	// 루트까지 score 업데이트, 맨 처음 호출한 노드는 제외(rollout 시에 호출 노드의 점수 업데이트 됨)
	Node* cur = this->parent_;
	while (cur != nullptr) {
		cur->UpdateScore(score);
		cur = cur->parent_;
	}
}

Node* Node::SelectChildByUct() {
	double best_uct = 0.0;
	Node* best_children = nullptr;

	for (Node* child : children_) {
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

double Node::CalculateUct() const {
	double reward_mean = (double)reward_sum_ / visit_cnt_;
	double exploration_term = sqrt(log(parent_->visit_cnt_) / visit_cnt_);
	return reward_mean + exploration_parameter_ * exploration_term;
}

// 병렬로 계산한 다른 트리의 children 계산 결과를 합침
void Node::MergeChildrenValues(Node* other) {
	reward_sum_ += other->reward_sum_;
	visit_cnt_ += other->visit_cnt_;
	for (uint i = 0; i < children_.size(); i++) {
		children_[i]->reward_sum_ += other->reward_sum_;
		children_[i]->visit_cnt_ += other->visit_cnt_;
	}
}

// best child 선택하여 배열의 인덱스를 리턴
// child 생성하는 순서가 항상 같으므로 인덱스가 모두 같음
// 따라서 인덱스가 같다면 같은 수를 두는 경우임
uint Node::SelectBestChild() const {
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

// Child 평가는 방문 수
uint Node::CalculateEvaluation() const {
	return visit_cnt_; 
}

// 최종 수를 선택할 때, 병렬 트리에서 가장 선택 많이 받은 수를 둠
Move Node::GetMostVotedMove(const std::vector<uint>& votes) const {
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
			// 투표수가 같다면, 방문을 더 많이 한 수를 선택함
			if (children_[i]->visit_cnt_ > visit) {
				visit = children_[i]->visit_cnt_;
				most_frequent_idx = i;
			}
		}
	}
	return children_[most_frequent_idx]->move_;
}