#include "mcts.hpp"
#include "node.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <sys/signal.h>

int main(int argc, char **argv) {
  constexpr uint32_t iter_count = 100;
  Board board;
  board.board = 0;
  board.merge_val = 0;
  board.SetAt(0, 1);
  board.SetAt(1, 1);

  Mcts mcts(board);
  std::random_device rd;
  std::mt19937 gen(rd());

  std::cout << "Statistics:\nNode size: " << sizeof(Node) << "b\nIteration count: " << iter_count << "\n\n";

  while(!board.IsTerminalState()) {
    board.MakeAction(mcts.CalculateBestAction(iter_count));
    std::vector<uint8_t> empty_tiles;

    for(int i = 0; i < 16; i++) {
      if(!board.GetAt(i))
        empty_tiles.emplace_back(i);
    }

    std::uniform_int_distribution<> dis1(0, empty_tiles.size() - 1);
    std::uniform_int_distribution<> dis2(0, 9);

    const int i_idx = empty_tiles.at(dis1(gen));
    int action = dis2(gen) < 9 ? 2 : 4;

    board.SetAt(i_idx, action);
    mcts.FindNodeByBoard(board);

    uint32_t merge_val = 1 << board.merge_val;

    std::cout << "Merge score: " << merge_val << " Tree size: " << mcts.GetTreeSize() << '\n';

    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
        int cell = (board.board >> ((i * 4 + j) * 4)) & 0xF;
        if(!cell) 
          std::cout << '0' << " ";
        else {
          uint32_t num = 1 << cell;
          std::cout << num << " ";
        }
      }
      std::cout << '\n';
    }
    std::cout << "\n\n";
  }
}

