#include "MonteCarloTree.h"

void MonteCarloTree::AddNodesUntilMaxDepth(uint max_depth) {
	RecursiveAddNodesUntilMaxDepth(*root_, 0, max_depth);
}

// Recursive: �� ��帶�� children�� ������
void MonteCarloTree::RecursiveAddNodesUntilMaxDepth(MonteCarloNode& node, uint cur_depth, uint max_depth) {
	if (cur_depth == max_depth) {
		return;
	}

	node.AddChildren();
	for (MonteCarloNode* child : node.GetChildren()) {
		RecursiveAddNodesUntilMaxDepth(*child, cur_depth + 1, max_depth);
	}
}

void MonteCarloTree::MonteCarloNode::AddChildren() {
	Turn next_turn;
	// ���� root ����� ���
	if (parent_ == nullptr) {
		next_turn = move_.turn;
	}
	else {
		next_turn = (move_.turn == Turn::Black) ? Turn::White : Turn::Black;
	}
	std::vector<Move> possible_moves = GetPossibleMoves(board_, next_turn);
	for (const Move& move : possible_moves) {
		children_.push_back(new MonteCarloNode(board_, move, exploration_parameter_, this));
	}
}

std::vector<Move> MonteCarloTree::MonteCarloNode::GetPossibleMoves(const Board& board, Turn turn)
{
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

void MonteCarloTree::Print() {
	std::cout << "===============" << std::endl;
	std::queue<MonteCarloNode*> q;
	q.push(root_);
	Turn turn_ = ai_turn_;
	while (!q.empty()) {
		MonteCarloNode* node = q.front(); q.pop();
		if (ai_turn_ != turn_) {
			turn_ = ai_turn_;
			std::cout << "===============" << std::endl;
		}
		node->PrintBoard();

		for (MonteCarloNode* child : node->GetChildren()) {
			q.push(child);
		}
	}
}

void MonteCarloTree::MonteCarloNode::PrintBoard() const {
	board_.Print();
}

std::vector<MonteCarloTree::MonteCarloNode*>& MonteCarloTree::MonteCarloNode::GetChildren() {
	return children_;
}

Move MonteCarloTree::GetMctsBestMove() {
	// Initialize
	MonteCarloNode* cur_node = root_;
	InitialRollout();

	// Ƚ�� ������ ���� �ʿ�
	uint rollout_cnt = 0;
	while (rollout_cnt < 100000) { // 12*12�� 100000 == �� 11��
		if (cur_node->IsGameOver()) {
			cur_node->Rollout();
			rollout_cnt++;
			cur_node = cur_node->GetParent();
			continue;
		}

		if (cur_node->IsLeafNode()) {
			if (cur_node->IsFirstVisit()) {
				cur_node->Rollout();
				rollout_cnt++;
			}
			else {
				// ���� ����̸� �̹� �� �� roll out
				cur_node->AddChildren();
				MonteCarloNode* first_child = cur_node->GetChildren().at(0);
				first_child->Rollout();
				rollout_cnt++;
			}
		}
		else {
			cur_node = cur_node->ChoseChildByUct();
		}
	}
	return root_->ChoseChildByUct()->GetMove();
}

void MonteCarloTree::InitialRollout()
{
	root_->RecursiveRollout();
}

void MonteCarloTree::MonteCarloNode::RecursiveRollout() {
	for (MonteCarloNode* node : children_) {
		if (node->IsLeafNode()) {
			node->Rollout();
		}
		else {
			node->RecursiveRollout();
		}
	}
}

bool MonteCarloTree::MonteCarloNode::IsGameOver() {
	return board_.IsGameOver();
}

Move MonteCarloTree::MonteCarloNode::GetMove() const {
	return move_;
}

Turn MonteCarloTree::MonteCarloNode::GetTurn() const {
	return move_.turn;
}

bool MonteCarloTree::MonteCarloNode::IsLeafNode() const {
	return children_.empty();
}

bool MonteCarloTree::MonteCarloNode::IsFirstVisit() const {
	return visit_cnt == 0;
}

void MonteCarloTree::MonteCarloNode::Rollout() {
	Board board = board_;
	Turn turn = move_.turn;

	while (!board.IsGameOver()) {
		turn = (turn == Turn::Black) ? Turn::White : Turn::Black;
		std::vector<Move> possible_moves = GetPossibleMoves(board, turn);
		// �õ尪�� ��� ���� random_device ����
		std::random_device rd;
		// random_device �� ���� ���� ���� ������ �ʱ�ȭ
		std::mt19937 gen(rd());
		// 0 ���� size ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����
		std::uniform_int_distribution<int> dis(0, possible_moves.size() - 1);
		uint index = dis(gen);
		Move next_move = possible_moves.at(index);
		board.PutNextMove(next_move);
	}
	Backpropagation(board.GetResult());
}

void MonteCarloTree::MonteCarloNode::Backpropagation(Turn winner) {
	// ��Ʈ���� UCT ������Ʈ
	MonteCarloNode* cur = this;
	while (cur != nullptr) {
		// �ϴ� �й� ���º� 0 �¸� 1
		int reward = 0;
		if (cur->GetTurn() == winner) {
			reward = 1;
		}
		else if (cur->GetTurn() == Turn::None) {
			reward = 0;
		}
		else {
			reward = -1;
		}
		cur->reward_sum_ += reward;
		cur->visit_cnt += 1;
		cur = cur->parent_;
	}
}

MonteCarloTree::MonteCarloNode* MonteCarloTree::MonteCarloNode::GetParent()
{
	return parent_;;
}

MonteCarloTree::MonteCarloNode* MonteCarloTree::MonteCarloNode::ChoseChildByUct() {
	MonteCarloNode* best_children = nullptr;
	double best_uct = 0.0;

	for (MonteCarloNode* child : children_) {
		if (child->visit_cnt == 0) {
			return child;
		}
		double uct = CalculateUct(visit_cnt, child->visit_cnt, child->reward_sum_);
		if (uct > best_uct) {
			best_uct = uct;
			best_children = child;
		}
	}
	return best_children;
}

double MonteCarloTree::MonteCarloNode::CalculateUct(uint np, uint nj, int reward_sum) {
	double reward_mean = reward_sum / nj;
	double exploration_term = sqrt(log(np) / nj);
	return reward_mean + exploration_parameter_ * exploration_term;
}