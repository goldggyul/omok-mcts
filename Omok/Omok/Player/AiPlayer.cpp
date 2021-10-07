#include "AiPlayer.h"

// 첫 수를 둘 경우만 따로 처리
Move AiPlayer::GetFirstMove(uint size) const {
	return Move(turn_, size / 2, size / 2);
}

Move AiPlayer::GetNextMove(const Omok& omok) {
	if (omok.GetMoveCount() == 0) {
		return GetFirstMove(omok.GetSize());
	}
	return SearchTree(omok);
}

Move AiPlayer::SearchTree(const Omok& omok) {
	// 탐색을 시작하기 위한 기본 트리
	Tree* first_tree = GetPartialTree(omok, 1, exploration_parameter_);

	// 트리의 개수, 각 트리를 병렬로 탐색 후에 결과를 합침
	uint tree_cnt = 16;
	if (tree_cnt % 2 == 0) {
		tree_cnt++;
	}

	std::vector<Tree*>* trees = first_tree->GetCopies(tree_cnt);
	SearchEachTree(trees);
	first_tree->MergeTreesValues(trees);

	// Voting Algorithm: 각 트리가 각자의 best에 투표하고, 가장 많은 표를 받은 수를 최종 best로 선택
	// Vote
	std::vector<uint> votes;
	for (Tree* tree : *trees) {
		votes.push_back(tree->GetBestMoveIndex());
	}
	// 결과
	Move next_move = first_tree->GetMostVotedMove(votes);

	DeleteTrees(trees);
	return next_move;
}

void AiPlayer::SearchEachTree(std::vector<Tree*>* trees) {
	// Root parallelization: thread 이용하여 병렬로 트리 탐색 진행
	std::vector<std::thread> workers;
	for (Tree* tree : *trees) {
		workers.push_back(std::thread(&Tree::Mcts, tree));
	}
	for (auto& worker : workers) {
		worker.join();
	}
}

void AiPlayer::DeleteTrees(std::vector<Tree*>* trees) {
	// 트리 각 노드들 메모리 해제는 다른 쓰레드에게 맡긴 후 detach(언제 종료되든 상관없으므로)
	// 삭제에 몇 초가 걸리기 때문
	std::thread tree_deletion([trees] {
		for (Tree* tree : *trees) {
			delete tree;
		}
		delete trees;
		});
	tree_deletion.detach();
}

// 인자인 최대 깊이까지 부분 트리를 생성
Tree* AiPlayer::GetPartialTree(const Omok& game_board, uint max_depth, double exploration_parameter) {
	// 트리 탐색 시작 시 최소 depth 1의 부분 트리 필요
	if (max_depth < 1) {
		max_depth = 1;
	}
	Tree* tree = new Tree(game_board, turn_, exploration_parameter);
	tree->AddNodesUntilMaxDepth(max_depth);
	return tree;
}
