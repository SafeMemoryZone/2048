#include "board.hpp"
#include "mcts.hpp"
#include "random_access_array.hpp"
#include <iomanip>
#include <iostream>
#include <random>

void SpawnRandomTile(Board &board) {
  std::random_device rd;
  std::mt19937 gen(rd());
  RandomAccessArray<int> empty_tiles;
  std::uniform_int_distribution<> tile_dis(0, 9);

  for (int i = 0; i < 16; i++) {
    if (!board.GetAt(i))
      empty_tiles.Add(i);
  }

  int idx = empty_tiles.GetRandomItemAndRemove();
  board.SetAt(idx, gen() < 9 ? 1 : 2);
}

void PrintBoard(const Board &board) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int cell = (board.board >> ((i * 4 + j) * 4)) & 0xF;
      if (!cell)
        std::cout << "   _" << " ";
      else {
        uint32_t num = 1 << cell;
        std::cout << std::setw(4) << num << " ";
      }
    }
    std::cout << '\n';
  }
}

int main(int argc, char **argv) {
  Board board;
  board.board = 0;
  board.merge_val = 0;
  SpawnRandomTile(board);
  SpawnRandomTile(board);

  Mcts mcts(board);

  while (!board.IsTerminalState()) {
    std::cout << "\n\n";
    int best_action = mcts.CalculateBestAction(1000);
    board.MakeAction(best_action);
    SpawnRandomTile(board);
    mcts.FindNodeByBoard(board);

    static std::array<std::string, 4> move_translation_map = {
      "up",
      "down",
      "left",
      "right"
    };

    std::cout << "[INFO] Merge value: " << board.merge_val << ", Move: " << move_translation_map.at(best_action) << "\n";
    PrintBoard(board);
  }

  return 0;
}
