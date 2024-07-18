#include "mcts.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <sys/signal.h>

int main(int argc, char **argv) {
  Board board;

  for(auto &row: board.board) {
    std::fill(row.begin(), row.end(), 0);
  }

  board.board.at(0).at(0) = 2;
  board.board.at(0).at(1) = 2;
  board.merge_val = 0;

  Mcts mcts(board);
  std::random_device rd;
  std::mt19937 gen(rd());

  while(!board.IsTerminalState()) {
    board.MakeAction(mcts.CalculateBestAction(6000));
    std::vector<std::pair<uint8_t, uint8_t>> empty_tiles;

    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
        if(!board.board.at(i).at(j))
          empty_tiles.emplace_back(i, j);
      }
    }

    std::uniform_int_distribution<> dis1(0, empty_tiles.size() - 1);
    std::uniform_int_distribution<> dis2(0, 9);

    const auto [i_idx, j_idx] = empty_tiles.at(dis1(gen));
    uint8_t action = dis2(gen) < 9 ? 2 : 4;

    board.board.at(i_idx).at(j_idx) = action;

    mcts.FindNodeByBoard(board);

    std::cout << "Merge score: " << board.merge_val << " Tree size: " << mcts.GetTreeSize() << '\n';
    for(const auto &row: board.board) {
      for(const auto cell: row) {
        std::cout << cell << ' ';
      }
      std::cout << '\n';
    }
    std::cout << "\n\n";
  }
}

