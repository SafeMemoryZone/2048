const timestamp = new Date().getTime();
const board_module_path = `./board.js?v=${timestamp}`;
const module = await import(board_module_path);
const GameBoard = module.default;

const start_button = document.getElementById("start-button");
start_button.addEventListener("click", startGame);

let game_board = null;
let last_key = null;

function startGame() {
  if (game_board !== null) {
    return;
  }

  // TODO: disable button
  start_button.disabled = true;
  game_board = new GameBoard();
  game_board.spawnRandomTile();
  game_board.spawnRandomTile();

  window.addEventListener("keydown", handleKeyPress);

  requestAnimationFrame(gameLoop);
}

function handleKeyPress(event) {
  switch (event.code) {
    case "ArrowRight":
      last_key = "right";
      break;
    case "ArrowLeft":
      last_key = "left";
      break;
    case "ArrowUp":
      last_key = "up";
      break;
    case "ArrowDown":
      last_key = "down";
      break;
    default:
      last_key = null;
      break;
  }
}

function gameLoop() {
  let add_tile = false;
  if (last_key !== null) {
    const saved_board = JSON.stringify(game_board.board);
    game_board.makeMove(last_key);

    if (saved_board === JSON.stringify(game_board.board)) {
      console.log("Keypress not allowed.");
    } else {
      console.log(`Keypress ${last_key} made.`);
      add_tile = true;
    }

    last_key = null;
  }

  if (!game_board.isTerminalState()) {
    if(add_tile) {
      game_board.spawnRandomTile();
    }
    requestAnimationFrame(gameLoop);
  } else {
    endGame();
  }
}

function endGame() {
  if (game_board === null) {
    return;
  }

  game_board = null;
  window.removeEventListener("keydown", handleKeyPress);
  start_button.disabled = false;
  // TODO: enable button and show game over screen
}
