#ifndef NODE_HPP
#define NODE_HPP
#include "board.hpp"
#include <cstdint>
#include "random_access_arr.hpp"

struct Node {
  double score;
  uint64_t visit_count;
  Node *parent;
  uint8_t action : 3;
  bool is_ai_turn : 1;
  bool should_soft_play : 1;
  bool is_expanded : 1;
  RandomAccessArr<uint8_t> unexpanded_actions;
  std::vector<Node*> children;
  Board board;
};
#endif // NODE_HPP
