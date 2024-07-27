function renderTile(i_idx, j_idx, tile) {
  const tile_cols = [
    '#eee4da', // color for tile 2
    '#ede0c8', // color for tile 4
    '#f2b179', // color for tile 8
    '#f59563', // color for tile 16
    '#f67c5f', // color for tile 32
    '#f65e3b', // color for tile 64
    '#edcf72', // color for tile 128
    '#edcc61', // color for tile 256
    '#edc850', // color for tile 512
    '#edc53f', // color for tile 1024
    '#edc22e', // color for tile 2048
    '#3c3a32', // color for tile 4096
  ];

  const tiles = document.querySelectorAll("#board > tbody > tr > td");
  const target = tiles[i_idx * 4 + j_idx];
  target.innerHTML = '';
  target.style.position = 'relative';
  target.style.borderRadius = '15px';

  const p = document.createElement("p");
  p.style.margin = 0;
  p.style.textAlign = "center";
  p.style.lineHeight = '100px';
  p.style.width = '100%';
  p.style.height = '100%';
  p.style.display = 'flex';
  p.style.alignItems = 'center';
  p.style.justifyContent = 'center';
  p.textContent = tile;

  target.appendChild(p);

  const col_idx = Math.min(Math.log2(tile) - 1, tile_cols.length - 1);
  target.style.backgroundColor = tile_cols[col_idx];
  p.style.backgroundColor = 'transparent';
}

class GameBoard {
  constructor() {
    this.board = [];
    this.#initGameBoard();
  }

  #initGameBoard() {
    for (let i = 0; i < 4; i++) {
      this.board.push([0, 0, 0, 0]);
    }
  }

  #generateDist(highest_num, lowest_num) {
    return () => Math.floor(Math.random() * (highest_num - lowest_num + 1)) + lowest_num;
  }

  spawnRandomTile() {
    const tile_dist = this.#generateDist(1, 10);
    const tile = tile_dist() < 9 ? 2 : 4;
    const empty_tiles = [];

    for (let i = 0; i < 4; i++) {
      for (let j = 0; j < 4; j++) {
        if (this.board[i][j] === 0) {
          empty_tiles.push([i, j]);
        }
      }
    }

    const pos_dist = this.#generateDist(0, empty_tiles.length - 1);
    const pos = empty_tiles[pos_dist()];
    this.board[pos[0]][pos[1]] = tile;
    renderTile(pos[0], pos[1], tile);
  }

  isTerminalState() {
    for (let i = 0; i < 4; i++) {
      for (let j = 0; j < 4; j++) {
        if (this.board[i][j] === 0) {
          return false;
        }

        // Check right neighbor
        if (j < 3 && this.board[i][j] === this.board[i][j + 1]) {
          return false;
        }

        // Check bottom neighbor
        if (i < 3 && this.board[i][j] === this.board[i + 1][j]) {
          return false;
        }
      }
    }

    return true;
  }

  makeMove(direction) {
    switch (direction) {
      case 'up':
        this.#moveUp();
        break;
      case 'down':
        this.#moveDown();
        break;
      case 'left':
        this.#moveLeft();
        break;
      case 'right':
        this.#moveRight();
        break;
    }
  }

  #moveUp() {
    for (let col = 0; col < 4; col++) {
      const merged = [false, false, false, false];
      for (let row = 1; row < 4; row++) {
        if (this.board[row][col] === 0) continue;
        let k = row;
        while (k > 0 && this.board[k - 1][col] === 0) {
          this.board[k - 1][col] = this.board[k][col];
          this.board[k][col] = 0;
          k--;
        }
        if (k > 0 && this.board[k - 1][col] === this.board[k][col] && !merged[k - 1]) {
          this.board[k - 1][col] *= 2;
          this.board[k][col] = 0;
          merged[k - 1] = true;
        }
      }
    }
  }

  #moveDown() {
    for (let col = 0; col < 4; col++) {
      const merged = [false, false, false, false];
      for (let row = 2; row >= 0; row--) {
        if (this.board[row][col] === 0) continue;
        let k = row;
        while (k < 3 && this.board[k + 1][col] === 0) {
          this.board[k + 1][col] = this.board[k][col];
          this.board[k][col] = 0;
          k++;
        }
        if (k < 3 && this.board[k + 1][col] === this.board[k][col] && !merged[k + 1]) {
          this.board[k + 1][col] *= 2;
          this.board[k][col] = 0;
          merged[k + 1] = true;
        }
      }
    }
  }

  #moveLeft() {
    for (let row = 0; row < 4; row++) {
      const merged = [false, false, false, false];
      for (let col = 1; col < 4; col++) {
        if (this.board[row][col] === 0) continue;
        let k = col;
        while (k > 0 && this.board[row][k - 1] === 0) {
          this.board[row][k - 1] = this.board[row][k];
          this.board[row][k] = 0;
          k--;
        }
        if (k > 0 && this.board[row][k - 1] === this.board[row][k] && !merged[k - 1]) {
          this.board[row][k - 1] *= 2;
          this.board[row][k] = 0;
          merged[k - 1] = true;
        }
      }
    }
  }

  #moveRight() {
    for (let row = 0; row < 4; row++) {
      const merged = [false, false, false, false];
      for (let col = 2; col >= 0; col--) {
        if (this.board[row][col] === 0) continue;
        let k = col;
        while (k < 3 && this.board[row][k + 1] === 0) {
          this.board[row][k + 1] = this.board[row][k];
          this.board[row][k] = 0;
          k++;
        }
        if (k < 3 && this.board[row][k + 1] === this.board[row][k] && !merged[k + 1]) {
          this.board[row][k + 1] *= 2;
          this.board[row][k] = 0;
          merged[k + 1] = true;
        }
      }
    }
  }
}

export default GameBoard;
