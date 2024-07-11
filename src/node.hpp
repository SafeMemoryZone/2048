#ifndef NODE_HPP
#define NODE_HPP
#include <cstdint>

struct Node {
  double score;
  double score_multiplier;
  uint64_t visit_count;
  uint64_t child_count;
  Node *children[32];
  Node *parent;
};
#endif // NODE_HPP
