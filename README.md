# AI
Here are my differents AI programs in C++ which implement and solve differents games like the minesweeper game, the sliding puzzle, sudoku, tic tac toe and the game of life

## Minesweeper
  I implemented the minesweeper, you have the choice to play the game or let the AI play by changing the value of the constant AI (line 10 from Démineur.cpp) to 1. If you want to play you can reveal a case with a left click and you can flag with a right click. The game is a 30x30 square randomly generated with 15% of bombs.
![AI Design](images/demineur1.JPG)

### Implementation of the AI : 
First i wanted to implement dpll algorithm in order to solve all the solvable minsweeper but I decided to just implement an AI using 2 simple rule and see its efficency:
First rule : 
![AI Design](images/demineur2.JPG)
