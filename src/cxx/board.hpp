#ifndef BOARD_HPP
#define BOARD_HPP
#include "random_access_array.hpp"
#include <cstdint>

#define DIRECTION_UP 0
#define DIRECTION_DOWN 1
#define DIRECTION_LEFT 2
#define DIRECTION_RIGHT 3

struct Board {
  bool IsTerminalState() const;
  void MakeAction(int action);
  RandomAccessArray<uint8_t> GetLegalActions();
  uint32_t GetBoardSum() const;
  void SetAt(int idx, int tile);
  int GetAt(int idx);

  uint32_t merge_val;
  uint64_t board;
};
#endif // BOARD_HPP 
