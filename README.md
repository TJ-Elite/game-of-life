# game-of-life
 C++ implementation of Conway's Game of Life.


## 1. Compatibility

This code has been tested to work in Visual Studio 2019 on Windows 7 compiling it using the ISO C++14 standard. An attempt to also provide Linux and Mac compatibility has been made, but this has not been tested to work on either operating system.

## 2. How to Use

When you run the application, you will first be asked to define the size of the NxN field. The minimum size is 3 and the maximum 200. Since the application uses the console window to display the cellural automaton, it is left up to the user to specify a grid that will fit in their console and be displayed correctly.

Once the grid size has been defined, you will next be asked to set the initial state of the board. It is initially filled with dead cells and you will be asked to specify which cells you want to set as alive. This is done by first providing an X-coordinate, followed by a Y-coordinate. The coordinates run from 1 to the size of the grid that you specified, with (1, 1) representing the top-left corner. After each live cell you add you can press ENTER to add another one or type "done" (without the quotes) and press ENTER to finish setting up the board.

Once the initial state has been set, you can from there on run the automaton manually by pressing ENTER to print each new generation to the screen. Typing "quit" and pressing ENTER will close the application. Alternatively you can type "auto" to run the automaton in auto mode where the screen will update every half a second. Please note that once in auto mode the application will no longer ask for any more inputs and you will have to close the application yourself.