#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <vector>

#define DIRECTION_UP 0
#define DIRECTION_DOWN 1
#define DIRECTION_LEFT 2
#define DIRECTION_RIGHT 3

struct Board {
  bool IsTerminalState();
  void MakeMove(int direction);
  std::vector<int> GetLegalMoves();
  Board MakeCopy();

  std::array<std::array<uint64_t, 4>, 4> board;
};
#endif // BOARD_HPP 
