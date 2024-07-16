#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>
#include <cstdint>
#include <vector>

#define DIRECTION_UP 0
#define DIRECTION_DOWN 1
#define DIRECTION_LEFT 2
#define DIRECTION_RIGHT 3

struct Board {
  bool IsTerminalState() const;
  void MakeMove(int direction);
  std::vector<int> GetLegalMoves() const;
  int CountOccupiedTiles() const;
  double GetBoardSum() const;

  uint64_t merge_val;
  std::array<std::array<uint64_t, 4>, 4> board;
};
#endif // BOARD_HPP 
