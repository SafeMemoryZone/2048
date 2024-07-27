#ifndef MCTS_HPP
#define MCTS_HPP
#include "board.hpp"
#include "node.hpp"

class Mcts {
public:
  explicit Mcts(const Board &board);
  ~Mcts();
  int CalculateBestAction(int iter_count);
  void FindNodeByBoard(const Board &board);

  inline uint32_t GetTreeSize() const {
    return this->tree_size;
  }

private:
  Node *tree_root; 
  double divisor;
  uint32_t tree_size;

  Node *GetBestUctChild(const Node *node);
  Node *SelectBestLeafNode(Node *node);
  Node *Expand(Node *node);
  bool IsLeafNode(const Node *node);
  double Simulate(const Node *node) const;
  void Backpropagate(Node *node, double eval) const;
  double GetUctScore(const Node *node);
  bool FindNodeWithCleanup(const Board &board, Node *node, int curr_level);
  void CleanupTreeFromRoot(Node *node);
};
#endif // MCTS_HPP
