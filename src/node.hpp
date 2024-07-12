#ifndef NODE_HPP
#define NODE_HPP
#include "board.hpp"
#include <cstdint>

struct Node {
  double score;
  double weight;
  uint64_t visit_count;
  uint64_t child_count;
  Node *children[32];
  Node *parent;
  int action;
  Board board;
  bool is_ai_turn;
};
#endif // NODE_HPP
