// © 2021 Tuukka Junnikkala

#include "game_of_life.h"
#include <iostream>
#include <string>
#include <limits>
#include <chrono>
#include <thread>

#if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN

#elif defined(__linux) || defined(__CYGWIN__)
#define OS_LINUX

#elif (defined(__APPLE__) || defined(__OSX__) || defined(__MACOS__)) && defined(__MACH__)
#define OS_MAC

#else
#error OS not supported!

#endif

#if defined(OS_WIN)
#define NOMINMAX
#include <windows.h> // NOMINMAX eliminates the clash between max() being defined in both "limits" and "windows.h".
#endif

using std::cout;
using std::cin;

/// <summary>
/// Runs Conway's Game of Life.
/// </summary>
void GameOfLife::run_game_of_life()
{
	cout << "Welcome to Conway's Game of Life.\n\n";
	uint16_t grid_size = ask_grid_size();
	std::vector<std::vector<uint8_t>> grid(initialize_grid(grid_size));
	set_initial_state(grid);
	run_automaton(grid);
}

/// <summary>
/// Asks the user for the size of the NxN board and keeps asking them until they provide a value that's within the accepted range.
/// </summary>
/// <returns>The length of one side of the board.</returns>
uint16_t GameOfLife::ask_grid_size()
{
	int grid_size = -1;
	do
	{
		cout << "Enter the size of the NxN field: ";
		if (!(cin >> grid_size))
		{
			cin.clear(); // Clear the failbit.
			cout << "You didn't enter a valid value!\n";
		}
		else if (grid_size < min_grid_size)
		{
			cout << "The minimum size of the field is " << min_grid_size << "!\n";
		}
		else if (grid_size > max_grid_size)
		{
			cout << "The maximum size of the field is " << max_grid_size << "!\n";
		}
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input.
	} while (grid_size < min_grid_size || grid_size > max_grid_size);

	return grid_size;
}

/// <summary>
/// Creates an NxN board full of dead cells.
/// </summary>
/// <param name="grid_size">The length of one side of the board.</param>
/// <returns>An NxN board filled with dead cells.</returns>
std::vector<std::vector<uint8_t>> GameOfLife::initialize_grid(uint16_t grid_size)
{
	std::vector<uint8_t> row(grid_size, 0);
	std::vector<std::vector<uint8_t>> grid(grid_size, row);

	return grid;
}

/// <summary>
/// Asks the user to set the initial state of the board by asking them for X- and Y-coordinates of cells they want to set as living.
/// At least one cell must be set alive.
/// </summary>
/// <param name="grid">The field of cells.</param>
void GameOfLife::set_initial_state(std::vector<std::vector<uint8_t>>& grid)
{
	cout << "\nNext you must set the initial state.\n";

	std::string user_input = "";
	do
	{
		// Ask the user for the X- and Y-coordinate of the cell they want to set as alive.
		Position cell{};
		cell.x = ask_coordinate('X', grid.size());
		cell.y = ask_coordinate('Y', grid[0].size());

		// Set cell as alive, but only if it's currently dead.
		if (get_cell_state(grid[cell.x][cell.y]) == 0)
		{
			update_grid(grid, cell, Cell_State::alive);
		}
		else
		{
			cout << "\nThe cell in (" << cell.x + 1 << ", " << cell.y + 1 << ") is already alive!\n";
		}

		cout << "\nCurrent initial state:\n\n";
		print_grid(grid);
		cout << '\n';

		cout << "Press ENTER to continue adding more living cells or type \"done\" followed by ENTER to ";
		cout << "finish setting the initial state: ";
		std::getline(cin, user_input);
	} while (user_input != "done" && user_input != "DONE");

	cout << "\nThe initial state has been set.\n\n";
}

/// <summary>
/// Asks the user for either an X- or Y- coordinate until they give a value that's within the board.
/// The coordinates asked from the user start from 1, but are mapped one value lower internally.
/// </summary>
/// <param name="axis">A character specifying which coordinate the user is being asked to input.</param>
/// <param name="grid_size">The length of one side of the board.</param>
/// <returns>The X- or Y-coordinate provided by the user.</returns>
uint16_t GameOfLife::ask_coordinate(char axis, unsigned int grid_size)
{
	int coordinate = 0;
	do
	{
		cout << "Enter the " << axis << "-coordinate of the living cell: ";
		if (!(cin >> coordinate))
		{
			cin.clear(); // Clear the failbit.
			cout << "You didn't enter a valid value!\n";
		}
		else if (coordinate < 1)
		{
			cout << "The coordinates start from 1!\n";
		}
		else if (coordinate > grid_size)
		{
			cout << "The size of the field is " << grid_size << "!\n";
		}
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input.
	} while (coordinate < 1 || coordinate > grid_size);

	// The coordinates provided by the user are shifted down by one.
	return coordinate - 1;
}

/// <summary>
/// Extracts whether the cell is alive or dead from the data stored in it.
/// </summary>
/// <param name="cell_content">The data of a single cell on the board.</param>
/// <returns>1 if the cell is alive, 0 if it is dead.</returns>
uint8_t GameOfLife::get_cell_state(uint8_t cell_content)
{
	// Bit position 0 is used to store the cell's state. 0 = dead, 1 = alive.
	return cell_content & 0b1;
}

/// <summary>
/// Updates the specified cell's state with the specified state. Neighboring cells' neighbor counts will also be incremented
/// or decremented by one accordingly.
/// </summary>
/// <param name="grid">The current field of cells.</param>
/// <param name="x">The X-coordinate of the cell that's to be updated.</param>
/// <param name="y">The Y-coordinate of the cell that's to be updated.</param>
/// <param name="new_cell_state">The cell's new state.</param>
void GameOfLife::update_grid(std::vector<std::vector<uint8_t>>& grid, uint16_t x, uint16_t y, Cell_State new_cell_state)
{
	Position cell = {x, y};
	update_grid(grid, cell, new_cell_state);
}

/// <summary>
/// Updates the specified cell's state with the specified state. Neighboring cells' neighbor counts will also be incremented
/// or decremented by one accordingly.
/// </summary>
/// <param name="grid">The current field of cells.</param>
/// <param name="cell">The position of the cell that's to be updated as an (X, Y) coordinate pair.</param>
/// <param name="new_cell_state">The cell's new state.</param>
void GameOfLife::update_grid(std::vector<std::vector<uint8_t>>& grid, Position cell, Cell_State new_cell_state)
{
	// Note that we don't check whether the new cell state differs from its current one.
	// This can lead to wrong neighbor counts if you try to update a cell with the state that matches its current one.
	update_cell(grid, cell, new_cell_state);
	update_neighbors(grid, cell, new_cell_state);
}

/// <summary>
/// Sets a cell's state as either alive or dead. Bit position 0 is used to store this data: 0 = dead, 1 = alive.
/// </summary>
/// <param name="grid">The current field of cells.</param>
/// <param name="cell">The cell that's to be updated.</param>
/// <param name="new_cell_state">The cell's new state.</param>
void GameOfLife::update_cell(std::vector<std::vector<uint8_t>>& grid, Position cell, Cell_State new_cell_state)
{
	if (new_cell_state == Cell_State::alive)
	{
		grid[cell.x][cell.y] |= 0b1; // Set cell as alive.
	}
	else
	{
		grid[cell.x][cell.y] &= 0b1111'1110; // Set cell as dead.
	}
}

/// <summary>
/// Updates the neighbor counts of the neighbors of the specified cell. If the updated cell's new state is alive, the neighbor counts
/// get incremented by one. If the new state is dead, they get decremented by one.
/// </summary>
/// <param name="grid">The current field of cells.</param>
/// <param name="cell">The cell that's been updated.</param>
/// <param name="new_cell_state">The cell's new state.</param>
void GameOfLife::update_neighbors(std::vector<std::vector<uint8_t>>& grid, Position cell, Cell_State new_cell_state)
{
	// Determine neighbor positions. The borders of the grid loop around to the opposite side.
	uint16_t x_left = cell.x > 0 ? cell.x - 1 : grid.size() - 1;
	uint16_t x_right = cell.x < grid.size() - 1 ? cell.x + 1 : 0;
	uint16_t y_above = cell.y > 0 ? cell.y - 1 : grid[0].size() - 1;
	uint16_t y_below = cell.y < grid[0].size() - 1 ? cell.y + 1 : 0;

	// Update neighbors' neighbor counts. This information is stored in the rest of the bits besides bit 0.
	// Because of this, adding or subtracting 0b10 (decimal value 2) increments or decrements the neighbor count by one respectively.
	grid[x_left][y_above] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
	grid[cell.x][y_above] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
	grid[x_right][y_above] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
	grid[x_left][cell.y] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
	grid[x_right][cell.y] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
	grid[x_left][y_below] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
	grid[cell.x][y_below] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
	grid[x_right][y_below] += new_cell_state == Cell_State::alive ? 0b10 : -0b10;
}

/// <summary>
/// Prints the field of cells to the console surrounded by a border to mark the edges.
/// </summary>
/// <param name="grid">The current field of cells.</param>
void GameOfLife::print_grid(const std::vector<std::vector<uint8_t>>& grid)
{
	// Print the top edge of the field.
	print_horizontal_border(grid.size());

	// Print the cells along with the left and right edge of the field.
	for (unsigned int y = 0; y < grid[0].size(); y++)
	{
		// Print '|' at the start of a line to mark the left edge.
		cout << '|';

		// Print one row of cells.
		for (unsigned int x = 0; x < grid.size(); x++)
		{
			if (get_cell_state(grid[x][y]) == 1)
			{
				cout << 'O'; // Live cells are indicated by 'O'.
			}
			else
			{
				cout << ' '; // An empty space denotes a dead cell.
			}
		}

		// Print '|' at the end of a line to mark the right edge.
		cout << '|' << std::endl;
	}

	// Print the bottom edge of the field.
	print_horizontal_border(grid.size());
}

/// <summary>
/// Prints a horizontal border of the specified width to the console, plus two additional characters for the corners.
/// </summary>
/// <param name="grid_size">The width of the field.</param>
void GameOfLife::print_horizontal_border(unsigned int grid_size)
{
	cout << '+'; // '+' is used to mark the corners.
	for (unsigned int i = 0; i < grid_size; i++)
	{
		cout << '-'; // '-' is used to mark horizontal edges of the field.
	}
	cout << '+' << std::endl;
}

/// <summary>
/// Runs the cellular automaton once the dimensions of the board have been set and it has been initialized. It can be run manually
/// a generation/tick at a time by pressing ENTER repeatedly or automatically by typing "auto" and pressing ENTER.
/// </summary>
/// <param name="grid">The current field of cells.</param>
void GameOfLife::run_automaton(std::vector<std::vector<uint8_t>>& grid)
{
	cout << "From this point on, you can keep pressing ENTER to manually display each new generation. ";
	cout << "Typing \"quit\" will allow you to close the application, while typing \"auto\" makes the cellural ";
	cout << "automaton start running continuously. When the automaton is running automatically, it will no ";
	cout << "longer ask for any new inputs from the user and you will have to close the program yourself.\n\n";
	cout << "Press ENTER to continue or type one of the available commands: ";

	// Run the automaton in manual mode until the user types "auto" or "quit".
	std::string user_input = "";
	do
	{
		std::getline(cin, user_input);
		if (user_input != "quit" && user_input != "QUIT")
		{
			one_tick(grid);
		}
	} while (user_input != "quit" && user_input != "QUIT" && user_input != "auto" && user_input != "AUTO");

	// Auto mode runs in an infinite loop with a set delay between each generation.
	while (user_input == "auto" || user_input == "AUTO")
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait 0.5s before printing each new generation.
		one_tick(grid);
	}

	cout << "\nGame of Life has now concluded. Press ENTER to close the application.";
	std::getline(cin, user_input);
}

/// <summary>
/// Clears the screen, prints the latest generation and calculates the next one.
/// </summary>
/// <param name="grid">The latest field of cells.</param>
void GameOfLife::one_tick(std::vector<std::vector<uint8_t>>& grid)
{
	clear_screen();
	print_grid(grid);
	next_generation(grid);
}

/// <summary>
/// Clears the console and moves the cursor back to its home position.
/// </summary>
void GameOfLife::clear_screen()
{
#if defined(OS_WIN)
	// For full disclosure, the Windows implementation below comes from https://www.cplusplus.com/articles/4z18T05o/.
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = {0, 0};

	if (hStdOut == INVALID_HANDLE_VALUE)
	{
		return;
	}

	// Get the number of cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		return;
	}
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire buffer with spaces.
	if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count))
	{
		return;
	}

	// Fill the entire buffer with the current colors and attributes.
	if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count))
	{
		return;
	}

	// Move the cursor home.
	SetConsoleCursorPosition(hStdOut, homeCoords);

#elif defined(OS_LINUX) || defined(OS_MAC)
	// Similar to Windows, this Linux/Mac implementation is also a result of some googling.
	// It shall be noted that this has not been tested on either Linux or Mac.

	// Use ANSI escape codes to clear the screen and move the cursor to home position.
	cout << "\x1B[2J\x1B[H";
#endif
}

/// <summary>
/// Update the state and neighbor count of each cell in the grid simultaneously based on their current state.
/// </summary>
/// <param name="grid">The current field of cells.</param>
void GameOfLife::next_generation(std::vector<std::vector<uint8_t>>& grid)
{
	// Create a copy of the current board.
	std::vector<std::vector<uint8_t>> next_grid(grid);

	// Update the new state of each cell first to the copied board based on the current board.
	for (unsigned int y = 0; y < grid[0].size(); y++)
	{
		for (unsigned int x = 0; x < grid.size(); x++)
		{
			// Any dead cell with exactly three live neighbors becomes a live cell.
			if (get_cell_state(grid[x][y]) == 0 && get_neighbor_count(grid[x][y]) == 3)
			{
				update_grid(next_grid, x, y, Cell_State::alive);
			}
			// Any live cell with fewer than two or more than three live neighbors dies.
			else if (get_cell_state(grid[x][y]) == 1 && (get_neighbor_count(grid[x][y]) < 2 || get_neighbor_count(grid[x][y]) > 3))
			{
				update_grid(next_grid, x, y, Cell_State::dead);
			}
		}
	}

	// Finally, copy the updated board to the one we are using.
	grid = next_grid;
}

/// <summary>
/// Extracts the neighbor count that's embedded in the contents of each cell.
/// </summary>
/// <param name="cell_content">The data of a single cell on the board.</param>
/// <returns>The number of live neighbors the cell currently has.</returns>
uint8_t GameOfLife::get_neighbor_count(uint8_t cell_content)
{
	// Neighbor count is stored in the rest of the bits besides bit position 0.
	return cell_content >> 1;
}
