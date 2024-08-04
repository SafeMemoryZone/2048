#include "board.hpp"
#include "random_access_array.hpp"
#include <cassert>

bool Board::IsTerminalState() const {
  for (int i = 0; i < 16; i++) {
    int curr_tile = (this->board >> (i * 4)) & 0xF;

    if (!curr_tile) 
      return false;

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
  switch(action) {
    case DIRECTION_UP:
      for(int col_idx = 0; col_idx < 4; col_idx++) {
        for(int row_idx = 0; row_idx < 4; row_idx++) {
          int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF;
          if(!curr_tile)
            continue;
          int k = row_idx;
          int next_tile = 0;

          while(k >= 1) {
            next_tile = (this->board >> ((k - 1) * 4 + col_idx) * 4) & 0xF;
            if(next_tile)
              break;
            k--;
          }

          if(next_tile && next_tile == curr_tile) {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset current tile
            next_tile++; // increment current tile
            this->board &= ~(0xFull << ((k - 1) * 4 + col_idx) * 4); // reset next tile
            this->board |= (uint64_t)next_tile << ((k - 1) * 4 + col_idx) * 4; // set next tile
            this->merge_val += 1ull << next_tile;
          }
          else {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4);
            this->board &= ~(0xFull << (k * 4 + col_idx) * 4);
            this->board |= (uint64_t)curr_tile << (k * 4 + col_idx) * 4;
          }
        }
      }
      break;
    case DIRECTION_DOWN:
      for(int col_idx = 0; col_idx < 4; col_idx++) {
        for(int row_idx = 3; row_idx >= 0; row_idx--) {
          int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF;
          if(!curr_tile)
            continue;
          int k = row_idx;
          int next_tile = 0;

          while(k < 3) {
            next_tile = (this->board >> ((k + 1) * 4 + col_idx) * 4) & 0xF;
            if(next_tile)
              break;
            k++;
          }

          if(next_tile && next_tile == curr_tile) {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset current tile
            next_tile++; // increment current tile
            this->board &= ~(0xFull << ((k + 1) * 4 + col_idx) * 4); // reset next tile
            this->board |= (uint64_t)next_tile << ((k + 1) * 4 + col_idx) * 4; // set next tile
            this->merge_val += 1ull << next_tile;
          }
          else {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4);
            this->board &= ~(0xFull << (k * 4 + col_idx) * 4);
            this->board |= (uint64_t)curr_tile << (k * 4 + col_idx) * 4;
          }
        }
      }
      break;
    case DIRECTION_LEFT:
      for(int row_idx = 0; row_idx < 4; row_idx++) {
        for(int col_idx = 0; col_idx < 4; col_idx++) {
          int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF;
          if(!curr_tile)
            continue;
          int k = col_idx;
          int next_tile = 0;

          while(k >= 1) {
            next_tile = (this->board >> (row_idx * 4 + k - 1) * 4) & 0xF;
            if(next_tile)
              break;
            k--;
          }

          if(next_tile && next_tile == curr_tile) {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset current tile
            next_tile++; // increment current tile
            this->board &= ~(0xFull << (row_idx * 4 + k - 1) * 4); // reset next tile
            this->board |= (uint64_t)next_tile << (row_idx * 4 + k - 1) * 4; // set next tile
            this->merge_val += 1ull << next_tile;
          }
          else {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4);
            this->board &= ~(0xFull << (row_idx * 4 + k) * 4);
            this->board |= (uint64_t)curr_tile << (row_idx * 4 + k) * 4;
          }
        }
      }
      break;
    case DIRECTION_RIGHT:
      for(int row_idx = 0; row_idx < 4; row_idx++) {
        for(int col_idx = 3; col_idx >= 0; col_idx--) {
          int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF;
          if(!curr_tile)
            continue;
          int k = col_idx;
          int next_tile = 0;

          while(k < 3) {
            next_tile = (this->board >> (row_idx * 4 + k + 1) * 4) & 0xF;
            if(next_tile)
              break;
            k++;
          }

          if(next_tile && next_tile == curr_tile) {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset current tile
            next_tile++; // increment current tile
            this->board &= ~(0xFull << (row_idx * 4 + k + 1) * 4); // reset next tile
            this->board |= (uint64_t)next_tile << (row_idx * 4 + k + 1) * 4; // set next tile
            this->merge_val += 1ull << next_tile;
          }
          else {
            this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4);
            this->board &= ~(0xFull << (row_idx * 4 + k) * 4);
            this->board |= (uint64_t)curr_tile << (row_idx * 4 + k) * 4;
          }
        }
      }
      break;
  }
}

void Board::SetAt(int idx, int tile) {
  this->board &= ~(0xFull << idx * 4);
  this->board |= (uint64_t)tile << idx * 4;
}

int Board::GetAt(int idx) {
  return (this->board >> idx * 4) & 0xF; 
}

uint32_t Board::GetBoardSum() const {
  uint32_t sum = 0;

  for (int i = 0; i < 16; i++) {
    int tile = (this->board >> i * 4) & 0xF;
    if (tile) 
      sum += 1ull << tile;
  }

  return sum;
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
