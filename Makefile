test:
	clang++ -o build/mcts -std=c++17 -g -fsanitize=undefined,address src/main.cc src/board.cc src/mcts.cc
