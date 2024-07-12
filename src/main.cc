#include "mcts.hpp"

int main(int argc, char **argv) {
  Board board;
  board.board.at(0).at(0) = 2;
  board.board.at(0).at(1) = 2;
  Mcts mcts(&board);
  board.MakeMove(mcts.CalculateBestAction(10000));
}
