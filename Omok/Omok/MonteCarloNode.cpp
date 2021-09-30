#include "MonteCarloNode.h"

// ��Ʈ ��忡�� ȣ��
// �ڽ� ���� �޸� �����ϰ� ���������� �ڽ� ��ü
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

// ��Ʈ ��忡�� ȣ��, MCTS ���� �� �κ� Ʈ���� ����
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

// ��� Rollout �Լ��� �������� �ڽ��� score�� ������Ʈ�ϰ� ����
// �� BackPropagtion�� �ʿ��ϴٸ� ���� ȣ��
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
	// �ڽ��� �� ������Ʈ
	UpdateScore(result);
	return result;
}

Score MonteCarloNode::RolloutLeafChild() {
	Score total_score;
	
	// future�̿�, ���ķ� ��� LeafChild�� rollout �Ŀ� ����� ��Ƽ� score ������Ʈ
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
	// �ڽ��� �� ������Ʈ
	UpdateScore(total_score);
	return total_score;
}

// cnt��ŭ�� child�� �������� �����Ͽ� rollout
Score MonteCarloNode::RandomRollout(uint cnt) {
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
	// �������� child ���� cnt�� ����
	for (uint i = 0; i < cnt; i++) {
		futures.push_back(std::async(&MonteCarloNode::Rollout, children_.at(i)));
	}
	Score total_score;
	for (auto& e : futures) {
		total_score += e.get();
	}
	// �ڽ��� �� ������Ʈ
	UpdateScore(total_score);
	return total_score;
}

bool MonteCarloNode::IsGameOver() {
	return omok_.IsGameOver();
}

void MonteCarloNode::UpdateScore(const Score& score) {
	uint reward = score.GetReward(move_.turn);
	reward_sum_ += reward;
	visit_cnt_ += score.GetVisitCnt();
}

void MonteCarloNode::AddChildren() {
	// ���� ������ ����Ǿ����Ƿ� child�� ���ϴ� ���� �ǹ̰� ����
	if (IsGameOver()) {
		return;
	}
	Turn next_turn;
	// ���� root ����� ���
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

// �ٸ� ���� ���������� �ʴٸ� ������ ����. ��, ���� ���� ���θ� üũ���� ����
std::vector<Move> MonteCarloNode::GetPossibleMoves(const Omok& board, Turn turn) {
	std::vector<Move> possible_moves;

	uint size = board.GetSize();
	bool** visited = new bool* [size];
	for (uint i = 0; i < size; i++) {
		visited[i] = new bool[size]();
	}

	// ������ ���� �ð�������� 
	int dx[8] = { 0,1,1,1,0,-1,-1,-1 };
	int dy[8] = { 1,1,0,-1,-1,-1,0,1 };
	for (uint i = 0; i < size; i++) {
		for (uint j = 0; j < size; j++) {
			if (!board.IsEmpty(i, j)) {
				// 8���� ���캽
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

void MonteCarloNode::Backpropagation(const Score& score) {
	// ��Ʈ���� score ������Ʈ, �� ó�� ȣ���� ���� ����
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

Move MonteCarloNode::ChoseBestMove() const {
	double best_eval = 0.0;
	MonteCarloNode* best_children = nullptr;

	for (MonteCarloNode* child : children_) {
		double eval = child->CalculateEvaluation();
		if (best_children == nullptr || eval > best_eval) {
			best_eval = eval;
			best_children = child;
		}
	}

	if (best_children == nullptr) {
		return Move(Turn::None, 0, 0);
	}
	return best_children->move_;
}

double MonteCarloNode::CalculateEvaluation() const {
	return (double)reward_sum_ / visit_cnt_;
}