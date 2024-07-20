#include "mcts.hpp"
#include "config.hpp"
#include <array>
#include <cassert>
#include <cfloat>
#include <cstdint>
#include <random>
#include <vector>

static void InitNewNode(Node *node, Node *parent, uint8_t action, const Board &board, bool is_ai_turn) {
  node->score = 0;
  node->visit_count = 0;
  node->parent = parent;
  node->action = action;
  node->board = board;
  node->is_ai_turn = is_ai_turn;
  node->is_expanded = false;
  node->unexpanded_actions.Clear();
  node->children.clear();
}

Mcts::Mcts(const Board &board) {
  this->tree_root = new Node;
  InitNewNode(this->tree_root, nullptr, UINT8_MAX, board, true);
  this->divisor = 1024;
  this->tree_size = 1;
}

Mcts::~Mcts() {
  if(this->tree_root)
    this->CleanupTreeFromRoot(this->tree_root);
}

uint8_t Mcts::CalculateBestAction(uint64_t iter_count) {
  std::random_device rd;
  std::mt19937 gen(rd());

  for (uint64_t i = 0; i < iter_count; i++) {
    Node *leaf = this->SelectBestLeafNode(this->tree_root);

    if(!leaf->board.IsTerminalState() && leaf->visit_count >= node_expansion_threshold)
      leaf = this->Expand(leaf);

    double eval = this->Simulate(leaf);
    this->Backpropagate(leaf, eval);
  }

  uint8_t best_action = UINT8_MAX;
  uint64_t max_visits = 0;

  for (const auto child: this->tree_root->children) {
    if (child->visit_count > max_visits) {
      best_action = child->action;
      max_visits = child->visit_count;
    }
  }

  assert(best_action != UINT8_MAX);
  return best_action;
}

Node *Mcts::GetBestUctChild(const Node *node) {
  double best_score = -DBL_MAX;
  Node *best_child = nullptr;

  for (const auto child: node->children) {
    double score = this->GetUctScore(child);
    if (score > best_score) {
      best_score = score;
      best_child = child;
    }
  }

  assert(best_child != nullptr);
  return best_child;
}

Node *Mcts::SelectBestLeafNode(Node *node) {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  if(this->IsLeafNode(node))
    return node;

  if(!node->is_ai_turn) {
    std::uniform_int_distribution<> tile_dis(0, 9);
    uint8_t action_choice = tile_dis(gen) < 9 ? 2 : 4;

    std::vector<Node *> possible_children;

    for(const auto child: node->children) {
      if(child->action == action_choice)
        possible_children.emplace_back(child);
    }

    std::uniform_int_distribution<> child_dis(0, possible_children.size() - 1);

    return this->SelectBestLeafNode(possible_children.at(child_dis(gen)));
  }

  return this->SelectBestLeafNode(this->GetBestUctChild(node));
}

bool Mcts::IsLeafNode(const Node *node) {
  // Node was expanded, doesn't have unexpanded actions nor children - terminal node
  if(node->is_expanded && !node->unexpanded_actions.Size() && !node->children.size())
    return true;

  // Node was expanded, doesn't have any unexpanded actions
  if(node->is_expanded && !node->unexpanded_actions.Size())
    return false;

  // Node wasn't expaneded or still has unexpanded actions
  return true;
}

Node *Mcts::Expand(Node *node) {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  if (node->is_ai_turn) {
    if(!node->is_expanded) {
      node->unexpanded_actions = node->board.GetLegalActions();
      node->is_expanded = true;
    }

    uint8_t action = node->unexpanded_actions.GetRandomItemAndRemove();
    
    Node *n = new Node;
    InitNewNode(n, node, action, node->board, false);
    n->board.MakeAction(action);
    this->tree_size++;

    node->children.emplace_back(n);

    return n;
  } 
  if(!node->is_expanded) {
    for(uint8_t i = 0; i < 4; i++) {
      for(uint8_t j = 0; j < 4; j++) {
        if(node->board.board.at(i).at(j))
          continue;

        // Legend:
        //  i - i index
        //  j - j index
        //  v - value of the tile
        // 1 Byte
        // +---+---+---+---+---+---+---+---+
        // | 0 | v | v | v | j | j | i | i |
        // +---+---+---+---+---+---+---+---+
        //   7   6   5   4   3   2   1   0
        // (MSB)                       (LSB)

        uint8_t action1 = (2 << 4) | (j << 2) | i;
        uint8_t action2 = (4 << 4) | (j << 2) | i;
        node->unexpanded_actions.Add(action1);
        node->unexpanded_actions.Add(action2);
      }
    }
    node->is_expanded = true;
  }

  uint8_t a = node->unexpanded_actions.GetRandomItemAndRemove();
  uint8_t i_idx = a & 0b11;
  uint8_t j_idx = (a & 0b1100) >> 2;
  uint8_t action = (a & 0b1110000) >> 4;

  Node *n = new Node;
  InitNewNode(n, node, action, node->board, true);
  n->board.board.at(i_idx).at(j_idx) = action;
  node->children.emplace_back(n);
  this->tree_size++;

  return n;
}

double Mcts::Simulate(const Node *node) const {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  Board board = node->board;
  bool is_environment_turn = !node->is_ai_turn;

  while (!board.IsTerminalState()) {
    if (is_environment_turn) {
      std::vector<std::pair<uint8_t, uint8_t>> empty_tiles;
      for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
          if (board.board.at(i).at(j) == 0)
            empty_tiles.emplace_back(i, j);
        }
      }
      std::uniform_int_distribution<> dis(0, empty_tiles.size() - 1);

      const auto [i_idx, j_idx] = empty_tiles.at(dis(gen));

      std::uniform_int_distribution<> dis2(0, 9);

      const uint8_t c = dis2(gen);
      if (c < 9) {
        board.board.at(i_idx).at(j_idx) = 2;
      } else {
        board.board.at(i_idx).at(j_idx) = 4;
      }
    } else {
      RandomAccessArray<uint8_t> legal_actions = board.GetLegalActions();
      board.MakeAction(legal_actions.GetRandomItemAndRemove());
    }

    is_environment_turn = !is_environment_turn;
  }

  return board.GetBoardSum();
}

void Mcts::Backpropagate(Node *node, double eval) const {
  Node *curr_n = node;

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

  double exploitation = node->score / node->visit_count / this->divisor;

  while(exploitation > 1) {
    this->divisor *= score_normalization_factor;
    exploitation = node->score / node->visit_count / this->divisor;
  }

  double exploration = c * sqrt(log(parent->visit_count) / node->visit_count);

  return exploration + exploitation;
}

void Mcts::FindNodeByBoard(const Board &board) {
  if(!this->FindNodeWithCleanup(board, this->tree_root, 0)) {
    this->CleanupTreeFromRoot(this->tree_root);
    this->tree_root = new Node;
    InitNewNode(this->tree_root, nullptr, UINT8_MAX, board, true);
    this->tree_size = 1;
  }
}

bool Mcts::FindNodeWithCleanup(const Board &board, Node *node, uint8_t curr_level) {
  if(curr_level > 2)
    return false;

  if (node->board.board == board.board) {
    for (const auto child: node->parent->children) {
      if (child != node) 
        this->CleanupTreeFromRoot(child);
    }
    node->parent = nullptr;
    this->tree_root = node;

    return true;
  } 

  for (const auto child: node->children) 
    if(this->FindNodeWithCleanup(board, child, curr_level + 1))
      return true;

  return false;
}

void Mcts::CleanupTreeFromRoot(Node *node) {
  assert(node != nullptr);

  for (const auto child: node->children) {
    this->CleanupTreeFromRoot(child);
  }

  delete node;
  this->tree_size--;
}
