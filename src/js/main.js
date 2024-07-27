import GameBoard from "./board.js";

const start_button = document.getElementById("start-button");
start_button.addEventListener("click", startGame);

let game_board = null;

function startGame() {
  if(game_board !== null) {
    return;
  }

  // disable button
  start_button.style.pointerEvents = "none";
  start_button.style.cursor = "default";
  start_button.style.backgroundColor = "lightgray"
  game_board = new GameBoard;
  game_board.spawnRandomTile();
  game_board.spawnRandomTile();
}
