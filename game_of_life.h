// © 2021 Tuukka Junnikkala

#ifndef GAME_OF_LIFE
#define GAME_OF_LIFE

#include <vector>

namespace GameOfLife
{
	// The application won't function correctly with a grid size smaller than 3.
	// Likewise, if the user specifies a grid that's wider than their console can display, things won't display correctly.
	// However, it is ultimately left up to the user to define a grid that's functional on their system.
	const uint16_t min_grid_size = 3;
	const uint16_t max_grid_size = 200;

	enum class Cell_State : uint8_t
	{
		dead,
		alive
	};

	struct Position
	{
		uint16_t x;
		uint16_t y;
	};

	void run_game_of_life();
	uint16_t ask_grid_size();
	std::vector<std::vector<uint8_t>> initialize_grid(uint16_t grid_size);
	void set_initial_state(std::vector<std::vector<uint8_t>>& grid);
	uint16_t ask_coordinate(char axis, unsigned int grid_size);
	uint8_t get_cell_state(uint8_t cell_content);
	void update_grid(std::vector<std::vector<uint8_t>>& grid, uint16_t x, uint16_t y, Cell_State new_cell_state);
	void update_grid(std::vector<std::vector<uint8_t>>& grid, Position cell, Cell_State new_cell_state);
	void update_cell(std::vector<std::vector<uint8_t>>& grid, Position cell, Cell_State new_cell_state);
	void update_neighbors(std::vector<std::vector<uint8_t>>& grid, Position cell, Cell_State new_cell_state);
	void print_grid(const std::vector<std::vector<uint8_t>>& grid);
	void print_horizontal_border(unsigned int grid_size);
	void run_automaton(std::vector<std::vector<uint8_t>>& grid);
	void one_tick(std::vector<std::vector<uint8_t>>& grid);
	void clear_screen();
	void next_generation(std::vector<std::vector<uint8_t>>& grid);
	uint8_t get_neighbor_count(uint8_t cell_content);
}

#endif // GAME_OF_LIFE
