test:
	clang++ -o build/mcts -std=c++17 -g -O3 src/main.cc src/board.cc src/mcts.cc
