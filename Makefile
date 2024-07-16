test:
	clang++ -o build/mcts -std=c++17 -O3 -fsanitize=undefined src/main.cc src/board.cc src/mcts.cc
