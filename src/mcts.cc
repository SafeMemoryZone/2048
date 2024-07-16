#include "mcts.hpp"
#include <array>
#include <cassert>
#include <cfloat>
#include <random>
#include <vector>

static void InitNewNode(Node *node, Node *parent, int action, const Board &board, bool is_ai_turn) {
  node->score = 0;
  node->visit_count = 0;
  node->child_count = 0;
  node->parent = parent;
  node->action = action;
  node->board = board;
  node->is_ai_turn = is_ai_turn;
}

Mcts::Mcts(const Board &board) {
  this->tree_root = this->AllocNode();
  InitNewNode(this->tree_root, nullptr, -1, board, true);
  this->divisor = 1024;
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
  bool is_soft_play = true;

  for (size_t i = 0; i < iter_count; i++) {
    Node *leaf = this->SelectBestLeafNode(this->tree_root);

    if(!leaf->board.IsTerminalState())
      this->Expand(leaf);

    if(leaf->child_count > 0) {
      std::uniform_int_distribution<> dis(0, leaf->child_count - 1);
      leaf = leaf->children[dis(gen)];
    }

    double eval = this->Simulate(leaf, is_soft_play);
    this->Backpropagate(leaf, eval);
    is_soft_play = !is_soft_play;
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

    for (int action: legal_actions) {
      Node *child_node = this->AllocNode();
      InitNewNode(child_node, node, action, node->board, false);
      child_node->board.MakeMove(action);
      node->children[node->child_count++] = child_node;
    }

    return;
  } 

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (!node->board.board.at(i).at(j)) {
        Node *child_node1 = this->AllocNode();
        InitNewNode(child_node1, node, 2, node->board, true);
        child_node1->board.board.at(i).at(j) = 2;

        Node *child_node2 = this->AllocNode();
        *child_node2 = *child_node1;
        child_node2->action = 4;
        child_node2->board.board.at(i).at(j) = 4;

        assert(node->child_count <= 30);
        node->children[node->child_count++] = child_node1;
        node->children[node->child_count++] = child_node2;
      }
    }
  }
}

double Mcts::Simulate(const Node *node, bool is_soft_play) const {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  Board board = node->board;
  bool is_environment_turn = !node->is_ai_turn;

  while (!board.IsTerminalState()) {
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
      if(is_soft_play) {
        std::vector<int> legal_moves = board.GetLegalMoves();
        std::uniform_int_distribution<> dis(0, legal_moves.size() - 1);

        board.MakeMove(legal_moves.at(dis(gen)));
      }
      else {
        int best_move = -1;
        double best_eval = -DBL_MAX;

        for(int move = 0; move < 4; move++) {
          Board test = board; 
          test.MakeMove(move);

          if(board.board == test.board) 
            continue;

          double val = test.GetBoardSum() / test.CountOccupiedTiles();

          if(val > best_eval) {
            best_eval = val;
            best_move = move;
          }
        }

        assert(best_move != -1);
        board.MakeMove(best_move);
      }
    }

    is_environment_turn = !is_environment_turn;
  }

  return board.GetBoardSum();
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

  double exploitation = node->score / node->visit_count / this->divisor;

  if(exploitation > 1)
    this->divisor *= 2;

  exploitation = node->score / node->visit_count / this->divisor;

  assert(exploitation <= 1);

  double exploration = c * sqrt(log(parent->visit_count) / node->visit_count);

  return exploration + exploitation;
}

void Mcts::FindNodeByBoard(const Board &board) {
  if(!this->FindNodeWithCleanup(board, this->tree_root, 0)) {

    this->CleanupTreeFromRoot(this->tree_root);
    this->tree_root = this->AllocNode();
    InitNewNode(this->tree_root, nullptr, -1, board, true);
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
  assert(node != nullptr);

  for (int i = 0; i < node->child_count; i++) {
    this->CleanupTreeFromRoot(node->children[i]);
  }

  this->free_nodes.emplace_back(node);
}
