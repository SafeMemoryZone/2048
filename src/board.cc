#include "board.hpp"
#include "random_access_array.hpp"
#include <cassert>

bool Board::IsTerminalState() const {
  for (int i = 0; i < 16; i++) {
    int curr_tile = (this->board >> (i * 4)) & 0xF;

    if (!curr_tile) {
      return false;
    }

    // Check right neighbor
    if ((i % 4) != 3) {
      int right_tile = (this->board >> ((i + 1) * 4)) & 0xF;
      if (curr_tile == right_tile) {
        return false;
      }
    }

    // Check bottom neighbor
    if (i < 12) {
      int bottom_tile = (this->board >> ((i + 4) * 4)) & 0xF;
      if (curr_tile == bottom_tile) {
        return false;
      }
    }
  }

  return true;
}

void Board::MakeAction(int action) {
  // TODO: Implement
  // NOTE: Update `merge_val += 1 << (tile_val + 1)`
}

void Board::SetAt(int idx, int tile) {
  this->board &= ~(0xFull << idx * 4);
  this->board |= (uint64_t)tile << idx * 4;
}

int Board::GetAt(int idx) {
  return (this->board >> idx * 4) & 0xF; 
}

uint32_t Board::GetBoardSum() const {
  uint64_t b = this->board;

  // Incrementally combine all tiles
  b = (b & 0x0F0F0F0F0F0F0F0F) + ((b >> 4) & 0x0F0F0F0F0F0F0F0F);
  b = (b & 0x00FF00FF00FF00FF) + ((b >> 8) & 0x00FF00FF00FF00FF);
  b = (b & 0x0000FFFF0000FFFF) + ((b >> 16) & 0x0000FFFF0000FFFF);
  b = (b & 0x00000000FFFFFFFF) + ((b >> 32) & 0x00000000FFFFFFFF);

  return b;
}

RandomAccessArray<uint8_t> Board::GetLegalActions() {
  // TODO: Optimize
  RandomAccessArray<uint8_t> actions;

  for(int action = 0; action < 4; action++) {
    Board test;
    test.merge_val = 0;
    test.board = this->board;

    test.MakeAction(action);

    if(this->board != test.board) 
      actions.Add(action);
  }

  return actions;
}
