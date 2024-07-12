#ifndef MCTS_HPP
#define MCTS_HPP
#include "board.hpp"
#include "node.hpp"
#include <deque>
#include <vector>

class Mcts {
public:
  explicit Mcts(const Board *initial_board);
  int CalculateBestAction(size_t iter_count);

private:
  Node *curr_node; 
  std::vector<Node *> free_nodes;
  std::deque<Node> arena;

  Node *AllocateNode();
  Node *GetBestUctChild(const Node *node) const;
  Node *SelectBestLeafNode(Node *node) const;
  Node *Expand(Node *node);
  bool IsFullyExpanded(const Node *node) const;
  double Simulate(const Node *node) const;
  void Backpropagate(Node *node, const double eval) const;
  double GetUctScore(const Node *node) const;
};
#endif // MCTS_HPP
