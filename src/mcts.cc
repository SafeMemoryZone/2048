#include "mcts.hpp"
#include <cassert>
#include <cfloat>
#include <numeric>
#include <random>
#include <vector>

// TODO: deallocate tree

Mcts::Mcts(const Board *initial_board) {
  this->curr_node = this->AllocateNode();
  this->curr_node->board = *initial_board;
  this->curr_node->action = -1;
  this->curr_node->is_ai_turn = true;
}

Node *Mcts::AllocateNode() {
  if (!this->free_nodes.size()) {
    this->arena.push_back(Node{0});
    return &this->arena.back();
  }

  Node *last = this->free_nodes.back();
  this->free_nodes.pop_back();

  assert(last != nullptr);
  return last;
}

int Mcts::CalculateBestAction(size_t iter_count) {
  for (size_t i = 0; i < iter_count; i++) {
    Node *leaf = this->SelectBestLeafNode(this->curr_node);

    if (!leaf->board.IsTerminalState()) {
      leaf = this->Expand(leaf);
    }

    double eval = this->Simulate(leaf);
    this->Backpropagate(leaf, eval);
  }

  int best_action = -1;
  size_t max_visits = 0;

  for (int i = 0; i < this->curr_node->child_count; i++) {
    const Node *child = this->curr_node->children[i];
    if (child->visit_count > max_visits) {
      best_action = child->action;
      max_visits = child->visit_count;
    }
  }

  assert(best_action != -1);
  return best_action;
}

Node *Mcts::GetBestUctChild(const Node *node) const {
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

Node *Mcts::SelectBestLeafNode(Node *node) const {
  if (!IsFullyExpanded(node))
    return node;

  return this->SelectBestLeafNode(this->GetBestUctChild(node));
}

Node *Mcts::Expand(Node *node) {
  std::random_device rd;
  std::mt19937 gen(rd());

  if (node->is_ai_turn) {
    std::vector<int> legal_actions = node->board.GetLegalMoves();
    std::vector<int> unexpanded_moves;

    assert(!legal_actions.empty());

    for (int action : legal_actions) {
      bool already_expanded = false;
      for (int i = 0; i < node->child_count; i++) {
        assert(node->children[i] != nullptr);
        if (node->children[i]->action == action) {
          already_expanded = true;
          break;
        }
      }
      if (!already_expanded) {
        unexpanded_moves.push_back(action);
      }
    }

    assert(!unexpanded_moves.empty());

    std::uniform_int_distribution<> dis(0, unexpanded_moves.size() - 1);
    int random_move = unexpanded_moves.at(dis(gen));

    Board new_board = node->board.MakeCopy();
    new_board.MakeMove(random_move);

    Node *child_node = this->AllocateNode();
    child_node->score = 0;
    child_node->weight = 1;
    child_node->visit_count = 0;
    child_node->child_count = 0;
    child_node->parent = node;
    child_node->action = random_move;
    child_node->board = new_board;
    child_node->is_ai_turn = false;

    node->children[node->child_count++] = child_node;
    return child_node;

  } else {
    std::vector<std::pair<int, int>> empty_tiles;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (node->board.board.at(i).at(j) == 0) {
          empty_tiles.emplace_back(i, j);
        }
      }
    }

    std::vector<std::pair<int, int>> unexpanded_tiles;

    for (const auto &tile : empty_tiles) {
      bool already_expanded = false;
      for (int i = 0; i < node->child_count; i++) {
        assert(node->children[i] != nullptr);
        if (node->children[i]->action == -1 && 
            node->children[i]->board.board.at(tile.first).at(tile.second) != 0) {
          already_expanded = true;
          break;
        }
      }
      if (!already_expanded) {
        unexpanded_tiles.push_back(tile);
      }
    }

    assert(!unexpanded_tiles.empty());

    std::uniform_int_distribution<> dis(0, unexpanded_tiles.size() - 1);
    const auto &[i_idx, j_idx] = unexpanded_tiles.at(dis(gen));

    std::uniform_int_distribution<> tile_dis(0, 9);
    int new_tile = tile_dis(gen) < 9 ? 2 : 4;

    Board new_board = node->board.MakeCopy();
    new_board.board.at(i_idx).at(j_idx) = new_tile;

    Node *child_node = this->AllocateNode();
    child_node->score = 0;
    child_node->weight = new_tile == 2 ? 0.9 : 0.1;
    child_node->visit_count = 0;
    child_node->child_count = 0;
    child_node->parent = node;
    child_node->action = -1;
    child_node->board = new_board;
    child_node->is_ai_turn = true;

    node->children[node->child_count++] = child_node;
    return child_node;
  }

  assert(false);
  return nullptr;
}

bool Mcts::IsFullyExpanded(const Node *node) const {
  if (node->is_ai_turn) {
    int empty_tiles_count = 0;
    for (const auto &row : node->board.board) {
      for (const auto tile : row) {
        if (!tile)
          empty_tiles_count++;
      }
    }

    if (node->child_count == empty_tiles_count * 2)
      return true;

    return false;
  }

  if (node->child_count == 4)
    return true;

  std::vector<int> legal_moves = node->board.GetLegalMoves();
  return node->child_count == legal_moves.size();
}

double Mcts::Simulate(const Node *node) const {
  std::random_device rd;
  std::mt19937 gen(rd());
  Board board = node->board.MakeCopy();
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
      std::vector<int> legal_moves = board.GetLegalMoves();
      std::uniform_int_distribution<> dis(0, legal_moves.size() - 1);

      board.MakeMove(legal_moves.at(dis(gen)));
    }

    is_environment_turn = !is_environment_turn;
  }

  double sum = 0;

  for (const auto &row : board.board) {
    sum += std::accumulate(row.begin(), row.end(), 0.0);
  }

  return sum;
}

void Mcts::Backpropagate(Node *node, double eval) const {
  auto curr_n = node;

  while (curr_n != nullptr) {
    eval *= curr_n->weight;

    curr_n->visit_count++;
    curr_n->score += eval;
    curr_n = curr_n->parent;
  }
}

static double Sigmoid(double x) {
  return 1 / (1 + std::exp(-x));
}

double Mcts::GetUctScore(const Node *node) const {
  const Node* parent = node->parent != nullptr ? node->parent : node;

  if (!node->visit_count)
    return DBL_MAX;

  constexpr double c = 1.44;
  double exploitation = Sigmoid((double)node->score) / node->visit_count;
  double exploration = c * sqrt(log(parent->visit_count) / node->visit_count);

  return exploration + exploitation;
}
