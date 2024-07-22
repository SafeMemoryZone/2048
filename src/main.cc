#include "mcts.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <sys/signal.h>

int main(int argc, char **argv) {
  Board board;

  board.board = 0;
  board.merge_val = 0;
  board.SetAt(0, 2);
  board.SetAt(1, 2);
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      int cell = (board.board >> ((i * 4 + j) * 4)) & 0xF;
      if(!cell) 
        std::cout << '0' << " ";
      else
        std::cout << std::pow(2, cell) << " ";
    }
    std::cout << '\n';
  }
  std::cout << "\n\n";

  auto l = board.GetLegalActions();
  board.board = 0;
  board.SetAt(1, 2);
  board.SetAt(4, 2);
  board.SetAt(8, 2);

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      int cell = (board.board >> (i * 4 + j) * 4) & 0xF;
      if(!cell) 
        std::cout << '0' << " ";
      else
        std::cout << std::pow(2, cell) << " ";
    }
    std::cout << '\n';
  }
  std::cout << "\n\n";


  l = board.GetLegalActions();

  board.MakeAction(l.GetRandomItemAndRemove());
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      int cell = (board.board >> (i * 4 + j) * 4) & 0xF;
      if(!cell) 
        std::cout << '0' << " ";
      else
        std::cout << std::pow(2, cell) << " ";
    }
    std::cout << '\n';
  }
  std::cout << "\n\n";
  return 0;

  Mcts mcts(board);
  std::random_device rd;
  std::mt19937 gen(rd());

  while(!board.IsTerminalState()) {
    board.MakeAction(mcts.CalculateBestAction(100));
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
        int cell = (board.board >> (i * 4 + j) * 4) & 0xF;
        if(!cell) 
          std::cout << '0' << " ";
        else
          std::cout << std::pow(2, cell) << " ";
      }
      std::cout << '\n';
    }
    std::cout << "\n\n";
  }
}

