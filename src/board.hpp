#ifndef BOARD_HPP
#define BOARD_HPP
#include "random_access_arr.hpp"
#include <array>
#include <cstdint>

#define DIRECTION_UP 0
#define DIRECTION_DOWN 1
#define DIRECTION_LEFT 2
#define DIRECTION_RIGHT 3

struct Board {
  bool IsTerminalState() const;
  void MakeAction(int action);
  RandomAccessArr<uint8_t> GetLegalActions() const;
  int CountOccupiedTiles() const;
  double GetBoardSum() const;

  uint64_t merge_val;
  std::array<std::array<uint32_t, 4>, 4> board;
};
#endif // BOARD_HPP 
