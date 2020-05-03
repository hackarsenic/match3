#pragma once

#include <random>
#include <vector>
#include <map>
#include <functional>

// Board's work logic goes here
class BoardLogic
{
public:
	BoardLogic(int columns, int rows);
	~BoardLogic();

	// generate random colors 
	void Generate();

	// swap source cell and target cell 
	bool SwapGemColors(int source_x, int sourceY, int targetX, int targetY);

	// Blows up the selected bomb
	bool DetonateBomb(int cell_x, int cell_y);

	// detects horizonal and vertical matching color patterns
	void DetectGemColorPatterns();

	// detects square matching color patterns
	void DetectGemSquareColorPatterns();

	// check if current board state contains patterns 
	bool HasPatterns();

	// remove matching color patterns 
	void RemovePattern();

	// adds colors to empty cells
	// empty cells move to top than get filled with colors
	void SpawnColors();

	// add bomb when formation detected
	void SpawnBomb(int cell_x, int cell_y);

	// returns the amount of columns
	const int& GetColumns();
	// returns the amount of rows
	const int& GetRows();
	// returns cell color
	const int GetGemColor(int column, int row);
	// returns amount of empty cells
	const int& GetEmptyCells();

	// methods for registering state changes (color changes)
	void ColorAddedHandler(std::function<void(int, int)> handlerFunction);
	void ColorRemovedHandler(std::function<void(int, int)> handlerFunction);
	void ColorSpawnedHandler(std::function<void(int, int)> handlerFunction);
	void BombAddedHandler(std::function<void(int, int)> handlerFunction);

private:
	// board size
	int _Columns;
	int _Rows;

	// vector holding color info of all cells
	std::vector<int> _Colors;

	// map holding bomb positions and type needed to spawn
	std::map<int, int> _BombPositions;

	// Number of empty cells
	int _EmptyCellsNum;

	int GetRandomColor();

	// check if selected color is part of patterns 
	bool IsPartOfSamePattern(int cell_x, int cell_y);

	// a bit array that describes valid color patterns on the board
	std::vector<bool> _ColorPatterns;

	// random number generation
	std::minstd_rand0 _RandomNumberGenerator;

	// uniform distribution for random number generation
	std::uniform_int_distribution<int> uint_dist;

	// handler functions
	std::function <void(int, int)> Color_Spawned;
	std::function <void(int, int)> Color_Added;
	std::function <void(int, int)> Color_Removed;
	std::function <void(int, int)> Bomb_Added;
};