test:
	clang++ -o build/mcts -std=c++17 -fsanitize=undefined -g src/main.cc src/board.cc src/mcts.cc
