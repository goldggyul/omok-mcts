#pragma once
#include <vector>
// for print tree
#include <queue>
#include "Player.h"

class AiPlayer : public Player {
public:
	AiPlayer(Turn turn_) :Player(turn_) {}
	Move GetFirstMove(const Board& game_board);
	virtual Move GetNextMove(const Board& game_board);

private:
	class Node {
	public:
		Node(const Board& board, Turn turn)
			:board_(board), turn_(turn), mean_reward_(0), parent_visit_cnt(0), visit_cnt(0), parent_(nullptr) {}

		Node(const Board& board, Turn turn, uint nx, uint ny)
			:Node(board, turn)
		{
			board_.PutNextMove(Move{ turn_, nx, ny });
		}

		void AddChild() {
			uint size = board_.GetSize();
			bool** visited = new bool* [size];
			for (uint i = 0; i < size; i++) {
				visited[i] = new bool[size]();
			}

			Turn next_turn = (turn_ == Turn::Black) ? Turn::White : Turn::Black;;
			// 오른쪽 부터 시계방향으로 
			int dx[8] = { 0,1,1,1,0,-1,-1,-1 };
			int dy[8] = { 1,1,0,-1,-1,-1,0,1 };
			for (uint i = 0; i < size; i++) {
				for (uint j = 0; j < size; j++) {
					if (!board_.IsEmpty(i, j)) {
						// 8방향 살펴봄
						for (uint d = 0; d < 8; d++) {
							uint nx = i + dx[d];
							uint ny = j + dy[d];
							if (nx < size && ny < size && board_.IsEmpty(nx, ny) && !visited[nx][ny]) {
								visited[nx][ny] = true;
								child_.push_back(new Node(board_, next_turn, nx, ny));
							}
						}
					}
				}
			}
		}

		void PrintBoard() {
			board_.Print();
		}

		Turn GetTurn() {
			return turn_;
		}

		std::vector<Node*>& GetChild() {
			return child_;
		}

	private:
		Board board_;
		Turn turn_;
		uint mean_reward_;
		uint parent_visit_cnt;
		uint visit_cnt;
		Node* parent_;
		std::vector<Node*> child_;
	};

	class Tree {
	public:
		Tree(Node* root_) :root_(root_) {}
		~Tree() {
			// TODO: Node들 메모리 해제
		}

		void AddNodesUntilMaxDepth(uint max_depth) {
			RecursiveAddNodesUntilMaxDepth(*root_, 0, max_depth);
		}

		void Print() {
			std::cout << "===============" << std::endl;
			std::queue<Node*> q;
			q.push(root_);
			Turn turn_ = root_->GetTurn();
			while (!q.empty()) {
				Node* node = q.front(); q.pop();
				if (node->GetTurn() != turn_) {
					turn_ = node->GetTurn();
					std::cout << "===============" << std::endl;
				}
				node->PrintBoard();

				for (Node* children : node->GetChild()) {
					q.push(children);
				}
			}
		}

	private:
		// Recursive: 각 노드마다 child를 더해줌
		void RecursiveAddNodesUntilMaxDepth(Node& node, uint cur_depth, uint max_depth) {
			if (cur_depth == max_depth) {
				return;
			}

			node.AddChild();
			for (Node* children : node.GetChild()) {
				RecursiveAddNodesUntilMaxDepth(*children, cur_depth + 1, max_depth);
			}
		}
		Node* root_;
	};

	Tree* GetPartialTree(const Board& game_board, uint max_depth);
};

