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

/* void Board::MakeAction(int action) { */
/*   // TODO: Optimize this mess */
/*   switch (action) { */
/*     case DIRECTION_UP: */
/*       for(int col_idx = 0; col_idx < 4; col_idx++) { */
/*         for(int row_idx = 0; row_idx < 4; row_idx++) { */
/*           int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF; */
/*           if(!curr_tile) */
/*             continue; */
/*           for(int k = row_idx - 1; k >= 0; k--) { */
/*             int above = (this->board >> (k * 4 + col_idx) * 4) & 0xF; */
/*             if(!above) */
/*               continue; */
/**/
/*             if(curr_tile == above) { */
/*               above++; */
/*               this->board &= ~(0xFull << (k * 4 + col_idx) * 4); */
/*               this->board |= (uint64_t)above << (k * 4 + col_idx) * 4; // set new above value */
/*               this->merge_val += curr_tile + 1; */
/*             } */
/**/
/*             else { */
/*               this->board &= ~(0xFull << ((k + 1) * 4 + col_idx) * 4); */
/*               this->board |= (uint64_t)curr_tile << ((k + 1) * 4 + col_idx) * 4; // set it the the current tile */
/*             } */
/**/
/*             this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset curr_tile place */
/*             break; */
/*           } */
/*         } */
/*       } */
/*       break; */
/**/
/*     case DIRECTION_DOWN: */
/*       for(int col_idx = 0; col_idx < 4; col_idx++) { */
/*         for(int row_idx = 3; row_idx >= 0; row_idx--) { */
/*           int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF; */
/*           if(!curr_tile) */
/*             continue; */
/*           for(int k = 0; k < row_idx; k++) { */
/*             int below = (this->board >> (k * 4 + col_idx) * 4) & 0xF; */
/*             if(!below) */
/*               continue; */
/**/
/*             if(curr_tile == below) { */
/*               below++; */
/*               this->board &= ~(0xFull << (k * 4 + col_idx) * 4); */
/*               this->board |= (uint64_t)below << (k * 4 + col_idx) * 4; // set new below value */
/*               this->merge_val += curr_tile + 1; */
/*             } */
/**/
/*             else { */
/*               this->board &= ~(0xFull << ((k - 1) * 4 + col_idx) * 4); */
/*               this->board |= (uint64_t)curr_tile << ((k - 1) * 4 + col_idx) * 4; // set it the the current tile */
/*             } */
/**/
/*             this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset curr_tile place */
/*             break; */
/*           } */
/*         } */
/*       } */
/*       break; */
/**/
/*     case DIRECTION_LEFT: */
/*       for(int row_idx = 0; row_idx < 4; row_idx++) { */
/*         for(int col_idx = 0; col_idx < 4; col_idx++) { */
/*           int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF; */
/*           if(!curr_tile) */
/*             continue; */
/*           for(int k = col_idx - 1; k >= 0; k--) { */
/*             int before = (this->board >> (row_idx * 4 + k) * 4) & 0xF; */
/*             if(!before) */
/*               continue; */
/**/
/*             if(curr_tile == before) { */
/*               before++; */
/*               this->board &= ~(0xFull << (row_idx * 4 + k) * 4); */
/*               this->board |= (uint64_t)before << (row_idx * 4 + k) * 4; // set new before value */
/*               this->merge_val += curr_tile + 1; */
/*             } */
/**/
/*             else { */
/*               this->board &= ~(0xFull << (row_idx * 4 + k + 1) * 4); */
/*               this->board |= (uint64_t)curr_tile << (row_idx * 4 + k + 1) * 4; // set it the the current tile */
/*             } */
/**/
/*             this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset curr_tile place */
/*             break; */
/*           } */
/*         } */
/*       } */
/*       break; */
/**/
/*     case DIRECTION_RIGHT: */
/*       for(int row_idx = 0; row_idx < 4; row_idx++) { */
/*         for(int col_idx = 3; col_idx >= 0; col_idx--) { */
/*           int curr_tile = (this->board >> (row_idx * 4 + col_idx) * 4) & 0xF; */
/*           if(!curr_tile) */
/*             continue; */
/*           for(int k = 0; k < col_idx; k++) { */
/*             int after = (this->board >> (row_idx * 4 + k) * 4) & 0xF; */
/*             if(!after) */
/*               continue; */
/**/
/*             if(curr_tile == after) { */
/*               after++; */
/*               this->board &= ~(0xFull << (row_idx * 4 + k) * 4); */
/*               this->board |= (uint64_t)after << (row_idx * 4 + k) * 4; // set new before value */
/*               this->merge_val += curr_tile + 1; */
/*             } */
/**/
/*             else { */
/*               this->board &= ~(0xFull << (row_idx * 4 + k - 1) * 4); */
/*               this->board |= (uint64_t)curr_tile << (row_idx * 4 + k - 1) * 4; // set it the the current tile */
/*             } */
/**/
/*             this->board &= ~(0xFull << (row_idx * 4 + col_idx) * 4); // reset curr_tile place */
/*             break; */
/*           } */
/*         } */
/*       } */
/*       break; */
/**/
/*     default: */
/*       assert(false); */
/*   } */
/* } */
void Board::MakeAction(int action) {
  auto CanCombineTiles = [this](int i1, int j1, int i2, int j2) -> bool {
    if (i1 < 0 || i1 > 3 || j1 < 0 || j1 > 3 || 
        i2 < 0 || i2 > 3 || j2 < 0 || j2 > 3) return false;

    int tile1 = (this->board >> ((i1 * 4 + j1) * 4)) & 0xF;
    int tile2 = (this->board >> ((i2 * 4 + j2) * 4)) & 0xF;

    return tile1 != 0 && tile1 == tile2;
  };

  auto GetTile = [this](int i, int j) -> int {
    return (this->board >> ((i * 4 + j) * 4)) & 0xF;
  };

  auto SetTile = [this](int i, int j, int value) {
    this->board &= ~(0xFull << ((i * 4 + j) * 4));
    this->board |= (uint64_t)value << ((i * 4 + j) * 4);
  };

  switch (action) {
    case DIRECTION_UP:
      for (int j = 0; j < 4; j++) {
        for (int i = 1; i < 4; i++) {
          int curr_tile = GetTile(i, j);
          if (curr_tile == 0) continue;
          int k = i;
          while (k > 0 && GetTile(k-1, j) == 0) {
            SetTile(k-1, j, curr_tile);
            SetTile(k, j, 0);
            k--;
          }
          if (k > 0 && CanCombineTiles(k, j, k-1, j)) {
            int new_value = GetTile(k-1, j) * 2;
            SetTile(k-1, j, new_value);
            SetTile(k, j, 0);
            this->merge_val += new_value;
          }
        }
      }
      break;

    case DIRECTION_DOWN:
      for (int j = 0; j < 4; j++) {
        for (int i = 2; i >= 0; i--) {
          int curr_tile = GetTile(i, j);
          if (curr_tile == 0) continue;
          int k = i;
          while (k < 3 && GetTile(k+1, j) == 0) {
            SetTile(k+1, j, curr_tile);
            SetTile(k, j, 0);
            k++;
          }
          if (k < 3 && CanCombineTiles(k, j, k+1, j)) {
            int new_value = GetTile(k+1, j) * 2;
            SetTile(k+1, j, new_value);
            SetTile(k, j, 0);
            this->merge_val += new_value;
          }
        }
      }
      break;

    case DIRECTION_LEFT:
      for (int i = 0; i < 4; i++) {
        for (int j = 1; j < 4; j++) {
          int curr_tile = GetTile(i, j);
          if (curr_tile == 0) continue;
          int k = j;
          while (k > 0 && GetTile(i, k-1) == 0) {
            SetTile(i, k-1, curr_tile);
            SetTile(i, k, 0);
            k--;
          }
          if (k > 0 && CanCombineTiles(i, k, i, k-1)) {
            int new_value = GetTile(i, k-1) * 2;
            SetTile(i, k-1, new_value);
            SetTile(i, k, 0);
            this->merge_val += new_value;
          }
        }
      }
      break;

    case DIRECTION_RIGHT:
      for (int i = 0; i < 4; i++) {
        for (int j = 2; j >= 0; j--) {
          int curr_tile = GetTile(i, j);
          if (curr_tile == 0) continue;
          int k = j;
          while (k < 3 && GetTile(i, k+1) == 0) {
            SetTile(i, k+1, curr_tile);
            SetTile(i, k, 0);
            k++;
          }
          if (k < 3 && CanCombineTiles(i, k, i, k+1)) {
            int new_value = GetTile(i, k+1) * 2;
            SetTile(i, k+1, new_value);
            SetTile(i, k, 0);
            this->merge_val += new_value;
          }
        }
      }
      break;

    default: assert(false);
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
