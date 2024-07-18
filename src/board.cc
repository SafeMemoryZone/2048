#include "board.hpp"
#include "random_access_array.hpp"
#include <cassert>
#include <numeric>

bool Board::IsTerminalState() const {
  static const int neighbour_offs[][2] = {
    {0, -1},
    {-1, 0},
    {0, 1},
    {1, 0}
  };

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      if(!this->board.at(i).at(j))
        return false;

      for(const auto [i_off, j_off]: neighbour_offs) {
        int new_i = i + i_off;
        int new_j = j + j_off;

        bool is_valid = new_i >= 0 && new_i < 4 && new_j >= 0 && new_j < 4;

        if(!is_valid)
          continue;

        if(this->board.at(new_i).at(new_j) == this->board.at(i).at(j))
          return false;
      }
    }
  }

  return true;
}

void Board::MakeAction(int action) {
  auto CanCombineTiles = [this](int i1, int j1, int i2, int j2) -> bool {
    if (i1 < 0 || i1 > 3 || j1 < 0 || j1 > 3 || 
        i2 < 0 || i2 > 3 || j2 < 0 || j2 > 3) return false;

    return this->board.at(i1).at(j1) != 0 && this->board.at(i1).at(j1) == this->board.at(i2).at(j2);
  };

  switch (action) {
    case DIRECTION_UP:
      for (int j = 0; j < 4; j++) {
        for (int i = 1; i < 4; i++) {
          if (this->board.at(i).at(j) == 0) continue;
          int k = i;
          while (k > 0 && this->board.at(k-1).at(j) == 0) {
            this->board.at(k-1).at(j) = this->board.at(k).at(j);
            this->board.at(k).at(j) = 0;
            k--;
          }
          if (k > 0 && CanCombineTiles(k, j, k-1, j)) {
            this->board.at(k-1).at(j) *= 2;
            this->board.at(k).at(j) = 0;
            this->merge_val += this->board.at(k-1).at(j);
          }
        }
      }
      break;
    case DIRECTION_DOWN:
      for (int j = 0; j < 4; j++) {
        for (int i = 2; i >= 0; i--) {
          if (this->board.at(i).at(j) == 0) continue;
          int k = i;
          while (k < 3 && this->board.at(k+1).at(j) == 0) {
            this->board.at(k+1).at(j) = this->board.at(k).at(j);
            this->board.at(k).at(j) = 0;
            k++;
          }
          if (k < 3 && CanCombineTiles(k, j, k+1, j)) {
            this->board.at(k+1).at(j) *= 2;
            this->board.at(k).at(j) = 0;
            this->merge_val += this->board.at(k+1).at(j);
          }
        }
      }
      break;
    case DIRECTION_LEFT:
      for (int i = 0; i < 4; i++) {
        for (int j = 1; j < 4; j++) {
          if (this->board.at(i).at(j) == 0) continue;
          int k = j;
          while (k > 0 && this->board.at(i).at(k-1) == 0) {
            this->board.at(i).at(k-1) = this->board.at(i).at(k);
            this->board.at(i).at(k) = 0;
            k--;
          }
          if (k > 0 && CanCombineTiles(i, k, i, k-1)) {
            this->board.at(i).at(k-1) *= 2;
            this->board.at(i).at(k) = 0;
            this->merge_val += this->board.at(i).at(k-1);
          }
        }
      }
      break;
    case DIRECTION_RIGHT:
      for (int i = 0; i < 4; i++) {
        for (int j = 2; j >= 0; j--) {
          if (this->board.at(i).at(j) == 0) continue;
          int k = j;
          while (k < 3 && this->board.at(i).at(k+1) == 0) {
            this->board.at(i).at(k+1) = this->board.at(i).at(k);
            this->board.at(i).at(k) = 0;
            k++;
          }
          if (k < 3 && CanCombineTiles(i, k, i, k+1)) {
            this->board.at(i).at(k+1) *= 2;
            this->board.at(i).at(k) = 0;
            this->merge_val += this->board.at(i).at(k+1);
          }
        }
      }
      break;
    default: assert(false);
  }
}

RandomAccessArray<uint8_t> Board::GetLegalActions() const {
  RandomAccessArray<uint8_t> legal_moves;

  for(uint8_t action = 0; action < 4; action++) {
    Board test = *this;
    test.MakeAction(action);
    if(test.board != this->board)
      legal_moves.Add(action);
  }

  return legal_moves;
}

int Board::CountOccupiedTiles() const {
  int count = 0;

  for(const auto &row: this->board)  {
    count += std::count_if(row.begin(), row.end(), [](auto x) {return x;} );
  }

  return count;
}

double Board::GetBoardSum() const {
  double accum = 0;

  for(const auto &row: this->board) {
    accum += std::accumulate(row.begin(), row.end(), 0.0);
  }

  return accum;
}
