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
	std::ofstream fout("uct_info.txt", std::ios::app);

	MonteCarloNode* cur_node = root_;
	auto start = std::chrono::steady_clock::now();
	InitialRollout();
	auto initial_rollout_end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(initial_rollout_end - start).count();
	fout << "     부분 트리 rollout: " << elapsed << "ms 경과" << std::endl;
	std::cout << "     부분 트리 rollout: " << elapsed << "ms 경과" << std::endl;

	while (true) {
		auto end = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - initial_rollout_end).count();
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
			Score score=cur_node->Rollout();
			rollout_cnt_++;
			cur_node->Backpropagation(score);
			cur_node = root_;
		}
		else {
			cur_node = cur_node->ChoseChildByUct();
		}
	}

	// for debugging
	fout << "     rollout 횟수: " << rollout_cnt_ << std::endl;
	std::cout << "     rollout 횟수: " << rollout_cnt_ << std::endl;
	fout.close();
	MonteCarloNode* best_child = root_->ChoseBestChild();
	std::cout <<"루트 방문 합: " << root_->GetVisitCnt() << std::endl;
	PrintRootAndChildrenMapAndUct(best_child);

	return best_child->GetMove();
}

void MonteCarloTree::InitialRollout()
{
	Score score = root_->RecursiveRollout();
}

Score MonteCarloTree::MonteCarloNode::RecursiveRollout() {
	Score total_score;
	std::mutex score_mtx;

	std::vector<std::thread> rollout_workers;
	for (MonteCarloNode* node : children_) {
		if (node->IsLeafNode()) {
			// v2. thread and mutex
			rollout_workers.push_back(std::thread([node, &total_score, &score_mtx] {
				Score score = node->Rollout();
				score_mtx.lock();
				total_score += score;
				score_mtx.unlock();
				}));

			// v1. no thread
			Score score = node->Rollout();
			total_score += score;
		}
		else {
			// v2. thread and mutex
			rollout_workers.push_back(std::thread([node, &total_score, &score_mtx] {
				Score score = node->RecursiveRollout();
				score_mtx.lock();
				total_score += score;
				score_mtx.unlock();
				}));

			// v1. no thread
			//Score score = node->RecursiveRollout();
			//total_score += score;
		}
	}

	// v2. thread and mutex
	for (auto& e : rollout_workers) {
		e.join();
	}
	UpdateScore(total_score);
	return total_score;
}

Score MonteCarloTree::MonteCarloNode::Rollout() {
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

void MonteCarloTree::MonteCarloNode::UpdateScore(const Score& score)
{
	int reward= score.GetReward(move_.turn);
	reward_sum_ += reward;
	visit_cnt += score.GetVisitCnt();
}

void MonteCarloTree::MonteCarloNode::Backpropagation(const Score& score) {
	// 루트까지 UCT 업데이트
	MonteCarloNode* cur = this->parent_;
	while (cur != nullptr) {
		cur->UpdateScore(score);
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
	std::cout << "child visit 총 합: "<<sum << std::endl;
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

MonteCarloTree::MonteCarloNode* MonteCarloTree::MonteCarloNode::GetParent() {
	return parent_;;
}

Turn MonteCarloTree::MonteCarloNode::GetTurn() const {
	return move_.turn;
}