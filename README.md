# Pentago Game

### About the Project:
Pentago is a two-player board game. The board consists of 4 rotating parts, which can be rotated. Each part contains 9 places for tokens. Players take turns making a move consisting of 2 parts:

- Placing their token on an unoccupied board space
- Rotating one part of the board by 90 degrees in any direction.
  
A player cannot skip any part of the move. 

The winner is the person who, after a full move, has 5 tokens arranged in a row/column/diagonally (similarly to tic-tac-toe).

### Program Usage:
The program uses the keyboard as follows:

- q, w, a, s - select the board part respectively: top-left, top-right, bottom-left, bottom-right.
- 1..9 - select the field on the board part as on the numeric keypad, i.e., 1 is the bottom-left field.
- z, x - rotate accordingly: clockwise, counterclockwise.
- p - pause.
- u - undo.
- o - load a predefined board.
- m - enter options.
- h - enter program description.
  
A move consists of first selecting the board part and field (e.g., q5), and then selecting the board part and rotation (e.g., az). Exit options p, m, h with any chosen command.

### Additional Information:
- Before starting the game, players' names should be entered.
- The game mode can be chosen - tic-tac-toe or pentago.
- The player's game time and token can be chosen (single character from the keyboard).
- Tokens cannot be placed on occupied spaces.
- The game can be paused.
- The program implements the function of undoing a move.
- After finishing the game, it can be reviewed.
- The program is protected against incorrectly entered data, e.g., player's double move, incorrect output data, etc.
