#include "mcts.hpp"
#include <array>
#include <cassert>
#include <cfloat>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

#define MAX_SIM_LEN 1000

// TODO: use proper score, better flattening than sigmoid

Mcts::Mcts(const Board &board) {
  Node *n = this->AllocNode();
  n->score = 0;
  n->visit_count = 0;
  n->child_count = 0;
  n->parent = nullptr;
  n->action = -1;
  n->board = board;
  n->is_ai_turn = true;

  this->tree_root = n;
}

Node *Mcts::AllocNode() {
  if (!this->free_nodes.size()) {
    this->arena.push_back(Node{});
    return &this->arena.back();
  }

  Node *last = this->free_nodes.back();
  this->free_nodes.pop_back();

  assert(last != nullptr);
  return last;
}

int Mcts::CalculateBestAction(size_t iter_count) {
  std::random_device rd;
  std::mt19937 gen(rd());

  for (size_t i = 0; i < iter_count; i++) {
    Node *leaf = this->SelectBestLeafNode(this->tree_root);
    this->Expand(leaf);

    if(leaf->child_count > 0) {
      std::uniform_int_distribution<> dis(0, leaf->child_count - 1);
      leaf = leaf->children[dis(gen)];
    }

    double eval = this->Simulate(leaf);
    this->Backpropagate(leaf, eval);
  }

  int best_action = -1;
  size_t max_visits = 0;

  for (int i = 0; i < this->tree_root->child_count; i++) {
    const Node *child = this->tree_root->children[i];
    if (child->visit_count > max_visits) {
      best_action = child->action;
      max_visits = child->visit_count;
    }
  }

  assert(best_action != -1);
  return best_action;
}

Node *Mcts::GetBestUctChild(const Node *node) {
  double best_score = -DBL_MAX;
  Node *best_child = nullptr;

  for (int i = 0; i < node->child_count; i++) {
    double score = this->GetUctScore(node->children[i]);
    if (score > best_score) {
      best_score = score;
      best_child = node->children[i];
    }
  }

  assert(best_child != nullptr);
  return best_child;
}

Node *Mcts::SelectBestLeafNode(Node *node) {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  if(!node->child_count)
    return node;

  if(!node->is_ai_turn) {
    std::uniform_int_distribution<> tile_dis(0, 9);
    int action_choice = tile_dis(gen) < 9 ? 2 : 4;

    std::vector<Node *> possible_children;

    for(int i = 0; i < node->child_count; i++) {
      if(node->children[i]->action == action_choice)
        possible_children.emplace_back(node->children[i]);
    }

    std::uniform_int_distribution<> child_dis(0, possible_children.size() - 1);
    
    return this->SelectBestLeafNode(possible_children.at(child_dis(gen)));
  }

  return this->SelectBestLeafNode(this->GetBestUctChild(node));
}

void Mcts::Expand(Node *node) {
  if (node->is_ai_turn) {
    std::vector<int> legal_actions = node->board.GetLegalMoves();

    for (int action : legal_actions) {
      Node *child_node = this->AllocNode();
      child_node->score = 0;
      child_node->visit_count = 0;
      child_node->child_count = 0;
      child_node->parent = node;
      child_node->action = action;
      child_node->board = node->board.MakeCopy();
      child_node->board.MakeMove(action);
      child_node->is_ai_turn = false;

      node->children[node->child_count++] = child_node;
    }

    return;
  } 

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (!node->board.board.at(i).at(j)) {
        Node *child_node1 = this->AllocNode();
        child_node1->score = 0;
        child_node1->visit_count = 0;
        child_node1->child_count = 0;
        child_node1->parent = node;
        child_node1->action = 2;
        child_node1->board = node->board.MakeCopy();
        child_node1->board.board.at(i).at(j) = 2;
        child_node1->is_ai_turn = true;

        Node *child_node2 = this->AllocNode();
        *child_node2 = *child_node1;
        child_node1->action = 4;
        child_node1->board.board.at(i).at(j) = 4;

        assert(node->child_count < 31);
        node->children[node->child_count++] = child_node1;
        node->children[node->child_count++] = child_node2;
      }
    }
  }
}

double Mcts::Simulate(const Node *node) const {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  Board board = node->board.MakeCopy();
  bool is_environment_turn = !node->is_ai_turn;

  int sim_i = 0;
  while (!board.IsTerminalState() && sim_i < MAX_SIM_LEN) {
    if (is_environment_turn) {
      std::vector<std::pair<int, int>> empty_tiles;
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          if (board.board.at(i).at(j) == 0)
            empty_tiles.emplace_back(i, j);
        }
      }
      std::uniform_int_distribution<> dis(0, empty_tiles.size() - 1);

      const auto [i_idx, j_idx] = empty_tiles.at(dis(gen));

      std::uniform_int_distribution<> dis2(0, 9);

      const int c = dis2(gen);
      if (c >= 0 && c <= 8) {
        board.board.at(i_idx).at(j_idx) = 2;
      } else {
        board.board.at(i_idx).at(j_idx) = 4;
      }
    } else {
      std::vector<int> legal_moves = board.GetLegalMoves();
      std::uniform_int_distribution<> dis(0, legal_moves.size() - 1);

      board.MakeMove(legal_moves.at(dis(gen)));
    }

    is_environment_turn = !is_environment_turn;
    sim_i++;
  }

  // TODO: flatten score
  double sum = 0;

  for (const auto &row : board.board) {
    sum += std::accumulate(row.begin(), row.end(), 0.0);
  }

  return sum;
}

void Mcts::Backpropagate(Node *node, double eval) const {
  auto curr_n = node;

  while (curr_n != nullptr) {
    curr_n->visit_count++;
    curr_n->score += eval;
    curr_n = curr_n->parent;
  }
}

double Mcts::GetUctScore(const Node *node) {
  const Node* parent = node->parent != nullptr ? node->parent : node;

  if (!node->visit_count)
    return DBL_MAX;

  constexpr double c = 1.44;
  double exploitation = (double)node->score / node->visit_count;
  double exploration = c * sqrt(log(parent->visit_count) / node->visit_count);

  return exploration + exploitation;
}

void Mcts::FindNodeByBoard(const Board &board) {
  if(!this->FindNodeWithCleanup(board, this->tree_root, 0)) {
    this->CleanupTreeFromRoot(this->tree_root);
    Node *n = this->AllocNode();
    n->score = 0;
    n->visit_count = 0;
    n->child_count = 0;
    n->parent = nullptr;
    n->action = -1;
    n->board = board;
    n->is_ai_turn = true;

    this->tree_root = n;
  }
}

bool Mcts::FindNodeWithCleanup(const Board &board, Node *node, int curr_level) {
  if(curr_level > 2)
    return false;

  if (node->board.board == board.board) {
    for (int i = 0; i < node->parent->child_count; i++) {
      Node *n = node->parent->children[i];
      if (n != node) 
        this->CleanupTreeFromRoot(n);
    }
    node->parent = nullptr;
    this->tree_root = node;

    return true;
  } 

  for (int i = 0; i < node->child_count; i++) 
    if(this->FindNodeWithCleanup(board, node->children[i], curr_level + 1))
      return true;

  return false;
}

void Mcts::CleanupTreeFromRoot(Node *node) {
  if (!node) return;

  for (int i = 0; i < node->child_count; i++) {
    this->CleanupTreeFromRoot(node->children[i]);
  }

  this->free_nodes.emplace_back(node);
}
