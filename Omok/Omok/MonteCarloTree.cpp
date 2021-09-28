#include "MonteCarloTree.h"

void MonteCarloTree::AddNodesUntilMaxDepth(uint max_depth) {
	RecursiveAddNodesUntilMaxDepth(*root_, 0, max_depth);
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

		for (MonteCarloNode* children : node->GetChild()) {
			q.push(children);
		}
	}
}

Move MonteCarloTree::GetMctsResult() {
	// Initialize
	MonteCarloNode* cur_node = root_;

	// Ƚ�� ������ ���� �ʿ�
	for (uint i = 0; i < 10000; i++) {
		if (cur_node->IsLeafNode()) {
			if (cur_node->IsFirstVisit()) {
				cur_node->Rollout();
			}
			else {
				cur_node->AddChild();
				// roll out ���� ���а� ��������..?
				MonteCarloNode* first_children = cur_node->GetChild().at(0);
				first_children->Rollout();
			}
		}
		else {
			cur_node = cur_node->ChoseChildrenByUcb();
		}
	}

	return Move();
}

// Recursive: �� ��帶�� child�� ������
void MonteCarloTree::RecursiveAddNodesUntilMaxDepth(MonteCarloNode& node, uint cur_depth, uint max_depth) {
	if (cur_depth == max_depth) {
		return;
	}

	node.AddChild();
	for (MonteCarloNode* children : node.GetChild()) {
		RecursiveAddNodesUntilMaxDepth(*children, cur_depth + 1, max_depth);
	}
}

void MonteCarloTree::MonteCarloNode::AddChild() {
	uint size = board_.GetSize();
	bool** visited = new bool* [size];
	for (uint i = 0; i < size; i++) {
		visited[i] = new bool[size]();
	}

	Turn next_turn;
	// ���� root ����� ���
	if (parent_ == nullptr) {
		next_turn = move_.turn;
	}
	else {
		next_turn = (move_.turn == Turn::Black) ? Turn::White : Turn::Black;
	}

	// ������ ���� �ð�������� 
	int dx[8] = { 0,1,1,1,0,-1,-1,-1 };
	int dy[8] = { 1,1,0,-1,-1,-1,0,1 };
	for (uint i = 0; i < size; i++) {
		for (uint j = 0; j < size; j++) {
			if (!board_.IsEmpty(i, j)) {
				// 8���� ���캽
				for (uint d = 0; d < 8; d++) {
					uint nx = i + dx[d];
					uint ny = j + dy[d];
					if (nx < size && ny < size && board_.IsEmpty(nx, ny) && !visited[nx][ny]) {
						visited[nx][ny] = true;
						child_.push_back(new MonteCarloNode(board_, Move{ next_turn, nx, ny }, this));
					}
				}
			}
		}
	}
}

bool MonteCarloTree::MonteCarloNode::IsFirstVisit() {
	return false;
}

void MonteCarloTree::MonteCarloNode::Rollout() {

}

void MonteCarloTree::MonteCarloNode::Backpropagation() {

}

MonteCarloTree::MonteCarloNode* MonteCarloTree::MonteCarloNode::ChoseChildrenByUcb() {
	return nullptr;
}

void MonteCarloTree::MonteCarloNode::PrintBoard() {
	board_.Print();
}

std::vector<MonteCarloTree::MonteCarloNode*>& MonteCarloTree::MonteCarloNode::GetChild() {
	return child_;
}

bool MonteCarloTree::MonteCarloNode::IsLeafNode() {
	return child_.empty();
}