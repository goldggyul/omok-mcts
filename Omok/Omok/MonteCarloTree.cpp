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

void MonteCarloTree::Mcts() {
	// Initialize
	std::ofstream fout("uct_info.txt", std::ios::app);

	MonteCarloNode* cur_node = root_;
	auto start = std::chrono::steady_clock::now();
	InitialRollout();
	auto initial_rollout_end = std::chrono::steady_clock::now();
	auto rollout_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(initial_rollout_end - start).count();
	
	fout << "     부분 트리 rollout: " << rollout_elapsed << "ms 경과" << std::endl;
	std::cout << "     부분 트리 rollout: " << rollout_elapsed << "ms 경과" << std::endl;
	while (true) {
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - initial_rollout_end).count();
		if (elapsed > 10000-rollout_elapsed) { // 10000ms(10초) 제한
			break;
		}
		if (cur_node->IsLeafNode()) {
			Score score;
			if (cur_node->IsFirstVisit()) {
				score = cur_node->Rollout();
			}
			else {
				cur_node->AddChildren();
				if (cur_node->IsLeafNode()) {
					score = cur_node->Rollout();
				}
				else {
					score = cur_node->RandomRollout();
					rollout_cnt_ += 7;
				}
			}
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
	std::cout << "루트 방문 합: " << root_->GetVisitCnt() << std::endl;
	//PrintRootAndChildrenMapAndUct(best_child);
}

Move MonteCarloTree::GetBestMove()
{
	MonteCarloNode* best_child = root_->ChoseBestChild();
	return best_child->GetMove();
}

void MonteCarloTree::MergeTreeValues(MonteCarloTree* other)
{
	root_->MergeRootNode(other->root_);
}

void MonteCarloTree::InitialRollout()
{
	root_->RolloutLeafChild();
}


void MonteCarloTree::WriteRootAndChildrenInfoToFile(MonteCarloNode* best_node) {

	std::ofstream fout("uct_info.txt", std::ios::app);
	fout << "------------------------------------------------------------------------" << std::endl;
	fout << "|  no. | 부모 방문 횟수 |  내 방문 횟수 | reward sum |  reward / visit  |" << std::endl;
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

void MonteCarloTree::PrintRootAndChildrenInfoToFile(MonteCarloNode* best_node) {
	std::cout << "------------------------------------------------------------------------" << std::endl;
	std::cout << "|  no. | 부모 방문 횟수 |  내 방문 횟수 | reward sum |  reward / visit  |" << std::endl;
	std::cout << "------------------------------------------------------------------------" << std::endl;

	std::cout << "------------------------------------------------------------------------" << std::endl;
	std::cout << "Root" << std::endl;
	root_->PrintBoard();

	uint cnt = 1;
	for (const auto* child : root_->GetChildren()) {
		if (child == best_node) {
			std::cout << "★";
		}
		else {
			// std::cout << std::endl;
			std::cout << "  |";
		}
		child->PrintInfo();
	}
	 std::cout << "------------------------------------------------------------------------" << std::endl;
}
