#ifndef MCTS_HPP
#define MCTS_HPP
#include "board.hpp"
#include "node.hpp"
#include <deque>
#include <vector>

class Mcts {
public:
  explicit Mcts(const Board &board);
  int CalculateBestAction(size_t iter_count);
  void FindNodeByBoard(const Board &board);

private:
  Node *tree_root; 
  std::vector<Node *> free_nodes;
  std::deque<Node> arena;
  double divisor;

  Node *AllocNode();
  Node *GetBestUctChild(const Node *node);
  Node *SelectBestLeafNode(Node *node);
  void Expand(Node *node);
  double Simulate(const Node *node, bool is_soft_play) const;
  void Backpropagate(Node *node, double eval) const;
  double GetUctScore(const Node *node);
  bool FindNodeWithCleanup(const Board &board, Node *node, int curr_level);
  void CleanupTreeFromRoot(Node *node);
};
#endif // MCTS_HPP
