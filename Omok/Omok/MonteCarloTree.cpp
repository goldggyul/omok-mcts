#include "MonteCarloTree.h"

void MonteCarloTree::AddNodesUntilMaxDepth(uint max_depth) {
	RecursiveAddNodesUntilMaxDepth(root_, 0, max_depth);
}

// Recursive: 각 노드마다 children를 더해줌
void MonteCarloTree::RecursiveAddNodesUntilMaxDepth(MonteCarloNode* node, uint cur_depth, uint max_depth) {
	if (node->IsGameOver()) {
		return;
	}
	if (cur_depth == max_depth) {
		return;
	}

	node->AddChildren();
	for (MonteCarloNode* child : node->GetChildren()) {
		RecursiveAddNodesUntilMaxDepth(child, cur_depth + 1, max_depth);
	}
}

void MonteCarloTree::MonteCarloNode::FreeTreeNode()
{
	RecursiveFreeNode();
	delete this;
}

void MonteCarloTree::MonteCarloNode::RecursiveFreeNode() {
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

void MonteCarloTree::MonteCarloNode::AddChildren() {
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

std::vector<Move> MonteCarloTree::MonteCarloNode::GetPossibleMoves(const Omok& board, Turn turn)
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

Move MonteCarloTree::GetMctsBestMove() {
	// Initialize
	MonteCarloNode* cur_node = root_;
	InitialRollout();

	auto start = std::chrono::steady_clock::now();
	long long elapsed=0;
	while (true) {
		auto end = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed > 10000) { // 10000ms(10초) 제한
			break;
		}
		if (cur_node->IsLeafNode()) {
			if (!(cur_node->IsFirstVisit())){
				// 리프 노드이며 이미 한 번 roll out
				cur_node->AddChildren();
				if (!cur_node->GetChildren().empty()) {
					cur_node = cur_node->GetChildren().at(0);
				}
			}
			cur_node->Rollout();
			rollout_cnt_++;
			cur_node = root_;
		}
		else {
			cur_node = cur_node->ChoseChildByUct();
		}
	}

	// for debugging
	std::ofstream fout("uct_info.txt", std::ios::app);
	fout << elapsed << "ms 경과 / " << "rollout 횟수: " << rollout_cnt_ << " / " << std::endl;
	std::cout << elapsed << "ms 경과 / " << "rollout 횟수: " << rollout_cnt_ << " / " << std::endl;
	fout.close();
	MonteCarloNode* best_child = root_->ChoseBestChild();
	PrintRootAndChildrenMapAndUct(best_child);

	return best_child->GetMove();
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

void MonteCarloTree::MonteCarloNode::Rollout() {
	Omok omok = omok_;
	Turn turn = move_.turn;

	while (!omok.IsGameOver()) {
		turn = (turn == Turn::Black) ? Turn::White : Turn::Black;
		std::vector<Move> possible_moves = GetPossibleMoves(omok, turn);
		// 시드값을 얻기 위한 random_device 생성
		std::random_device rd;
		// random_device 를 통해 난수 생성 엔진을 초기화
		std::mt19937 gen(rd());
		// 0 부터 size 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의
		std::uniform_int_distribution<int> dis(0, possible_moves.size() - 1);

		uint index = dis(gen);
		Move next_move = possible_moves.at(index);
		omok.PutNextMove(next_move);
	}
	Backpropagation(omok.GetResult());
}

void MonteCarloTree::MonteCarloNode::Backpropagation(Turn winner) {
	// 루트까지 UCT 업데이트
	MonteCarloNode* cur = this;
	while (cur != nullptr) {
		// 일단 패배 무승부 0 승리 1
		int reward = 0;
		if (cur->GetTurn() == winner) {
			reward = 1;
		}
		cur->reward_sum_ += reward;
		cur->visit_cnt += 1;
		cur = cur->parent_;
	}
}

MonteCarloTree::MonteCarloNode* MonteCarloTree::MonteCarloNode::ChoseChildByUct() {
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

double MonteCarloTree::MonteCarloNode::CalculateUct() const {
	double reward_mean = (double)reward_sum_ / visit_cnt;
	double exploration_term = sqrt(log(parent_->visit_cnt) / visit_cnt);
	return reward_mean + exploration_parameter_ * exploration_term;
}

MonteCarloTree::MonteCarloNode* MonteCarloTree::MonteCarloNode::ChoseBestChild() {
	double best_eval = 0.0;
	MonteCarloNode* best_children = nullptr;

	for (MonteCarloNode* child : children_) {
		double eval = child->CalculateEvaluation();
		if (best_children == nullptr || eval > best_eval) {
			best_eval = eval;
			best_children = child;
		}
	}
	return best_children;
}

double MonteCarloTree::MonteCarloNode::CalculateEvaluation() const
{
	return (double)reward_sum_/visit_cnt;
}

void MonteCarloTree::PrintRootAndChildrenMapAndUct(MonteCarloNode* best_node) {

	std::ofstream fout("uct_info.txt", std::ios::app);
	fout << "------------------------------------------------------------------------" << std::endl;
	fout << "|  no. | 부모 방문 횟수 |  내 방문 횟수 | reward sum |       UCT       |" << std::endl;
	fout << "------------------------------------------------------------------------" << std::endl;

	//std::cout << "------------------------------------------------------------------------" << std::endl;
	//std::cout << "Root" << std::endl;
	//root_->PrintBoard();

	uint cnt = 1;
	for (const auto* child : root_->GetChildren()) {
		// std::cout << cnt << "번째 child";
		fout << "|" << std::setw(6) << cnt;
		cnt++;
		if (child == best_node) {
			// std::cout << "★" << std::endl;
			fout << "★";
		}
		else {
			// std::cout << std::endl;
			fout << "  |";
		}
		child->PrintInfo(fout);
		// child->PrintBoard();
	}
	// std::cout << "------------------------------------------------------------------------" << std::endl;
	fout << "------------------------------------------------------------------------" << std::endl;
	fout.close();

}

void MonteCarloTree::MonteCarloNode::PrintBoard() const {
	omok_.Print();
}

void MonteCarloTree::MonteCarloNode::PrintInfo(std::ofstream& fout) const
{
	//std::cout << std::setw(16) << parent_->visit_cnt << "|" << std::setw(17) << visit_cnt << "|" << std::setw(16) << reward_sum_ << "|" << std::setw(16) << CalculateUct() << "|" << std::endl;
	fout << std::setw(16) << parent_->visit_cnt << "|" << std::setw(17) << visit_cnt << "|" << std::setw(16) << reward_sum_ << "|" << std::setw(16) << CalculateEvaluation() << "|" << std::endl;
}

std::vector<MonteCarloTree::MonteCarloNode*>& MonteCarloTree::MonteCarloNode::GetChildren() {
	return children_;
}

bool MonteCarloTree::MonteCarloNode::IsGameOver() {
	return omok_.IsGameOver();
}

Move MonteCarloTree::MonteCarloNode::GetMove() const {
	return move_;
}

bool MonteCarloTree::MonteCarloNode::IsLeafNode() const {
	return children_.empty();
}

bool MonteCarloTree::MonteCarloNode::IsFirstVisit() const {
	return visit_cnt == 0;
}

MonteCarloTree::MonteCarloNode* MonteCarloTree::MonteCarloNode::GetParent()
{
	return parent_;;
}

Turn MonteCarloTree::MonteCarloNode::GetTurn() const {
	return move_.turn;
}