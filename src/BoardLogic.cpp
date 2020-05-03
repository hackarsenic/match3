#include "BoardLogic.hpp"
#include "Gem.hpp""


BoardLogic::BoardLogic(int columns, int rows) :
	_Columns(columns),
	_Rows(rows),
	_EmptyCellsNum(0)
{
	// resize vectors according to board size
	_Colors.resize(_Columns * _Rows, -1);
	_ColorPatterns.resize(_Columns * _Rows, false);

	//seed with time
	_RandomNumberGenerator.seed(time(NULL));


}

BoardLogic::~BoardLogic()
{
	_Colors.clear();
	_ColorPatterns.clear();
}

bool BoardLogic::SwapGemColors(int source_x, int source_y, int target_x, int target_y)
{
	int source_position = source_y * _Columns + source_x;
	int target_position = target_y * _Columns + target_x;

	// try to swap colors - check for pattern
	int source_color = _Colors[source_y * _Columns + source_x];
	int target_color = _Colors[target_y * _Columns + target_x];

	// swap
	_Colors[source_position] = target_color;
	_Colors[target_position] = source_color;

	// check if any of selected colors is now part of a pattern
	if (IsPartOfSamePattern(source_x, source_y) || IsPartOfSamePattern(target_x, target_y))
	{
		return true;
	}
	else
	{
		// reverse swap
		_Colors[source_position] = source_color;
		_Colors[target_position] = target_color;

		return false;
	}
}

bool BoardLogic::DetonateBomb(int cell_x, int cell_y)
{
	// Square bomb detonation case
	if (_Colors[cell_y * _Columns + cell_x] == 8)
	{
		auto is_valid = [this](int row, int col) -> bool
		{
			return (row >= 0) && (row < _Rows) &&
				(col >= 0) && (col < _Columns);
		};


		// checks for edge cases
		if (is_valid(cell_y - 1, cell_x - 1))
		{
			_Colors[(cell_y - 1) * _Columns + cell_x - 1] = -1;
			Color_Removed(cell_x - 1, cell_y - 1);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y - 1, cell_x))
		{
			_Colors[(cell_y - 1) * _Columns + cell_x] = -1;
			Color_Removed(cell_x, cell_y - 1);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y - 1, cell_x + 1))
		{
			_Colors[(cell_y - 1) * _Columns + cell_x + 1] = -1;
			Color_Removed(cell_x + 1, cell_y - 1);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y, cell_x - 1))
		{
			_Colors[cell_y * _Columns + cell_x - 1] = -1;
			Color_Removed(cell_x - 1, cell_y);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y, cell_x))
		{
			_Colors[cell_y * _Columns + cell_x] = -1;
			Color_Removed(cell_x, cell_y);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y, cell_x + 1))
		{
			_Colors[cell_y * _Columns + cell_x + 1] = -1;
			Color_Removed(cell_x + 1, cell_y);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y + 1, cell_x - 1))
		{
			_Colors[(cell_y + 1) * _Columns + cell_x - 1] = -1;
			Color_Removed(cell_x - 1, cell_y + 1);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y + 1, cell_x))
		{
			_Colors[(cell_y + 1) * _Columns + cell_x] = -1;
			Color_Removed(cell_x, cell_y + 1);
			++_EmptyCellsNum;
		}

		if (is_valid(cell_y + 1, cell_x + 1))
		{
			_Colors[(cell_y + 1) * _Columns + cell_x + 1] = -1;
			Color_Removed(cell_x + 1, cell_y + 1);
			++_EmptyCellsNum;
		}
		return true;
	}


	// detonate vertical bomb
	if (_Colors[cell_y * _Columns + cell_x] == 6)
	{
		for (int i = 0; i < _Rows; ++i)
		{
			_Colors[i * _Columns + cell_x] = -1;
			Color_Removed(cell_x, i * _Columns);
			++_EmptyCellsNum;
		}
		return true;
	}

	// detonate horizontal bomb
	if (_Colors[cell_y * _Columns + cell_x] == 7)
	{
		for (int i = 0; i < _Columns; ++i)
		{
			_Colors[i + cell_y * _Columns] = -1;
			Color_Removed(i, cell_y * _Columns);
			++_EmptyCellsNum;
		}
		return true;
	}
	return false;
}

int BoardLogic::GetRandomColor()
{
	return uint_dist(_RandomNumberGenerator) % (int)Gem::Color::COUNT;
}

void BoardLogic::Generate()
{
	// check that the two pieces to the left or two pieces down are not the same color as the gem added
	for (int y = 0; y < _Rows; ++y)
	{
		for (int x = 0; x < _Columns; ++x)
		{
			int position = y * _Columns + x;

			int new_color = GetRandomColor();
			do
			{
				new_color = GetRandomColor();
			} while ((x > 1 &&
				_Colors[(x - 1) + (y * _Columns)] == new_color &&
				_Colors[(x - 2) + (y * _Columns)] == new_color)
				||
				(y > 1 &&
					_Colors[x + ((y - 1) * _Columns)] == new_color &&
					_Colors[x + ((y - 2) * _Columns)] == new_color)
				|| (x > 1 && y > 1) && _Colors[x + (y - 1) * _Columns] == new_color);
			_Colors[position] = new_color;
		}
	}
	_EmptyCellsNum = 0;
}

void BoardLogic::DetectGemColorPatterns()
{
	// scan board rows to find horizontal color pattern
	for (int y = 0; y < _Rows; ++y)
	{
		for (int x = 0; x < _Columns; ++x)
		{
			int position = y * _Columns + x;
			int color = _Colors[position];

			// look in this row for a pattern
			int pattern_count_x = 1;
			int pattern_count_y = 1;

			// look horizontally
			int matching_color = color;

			while (true)
			{
				int search_position = position + pattern_count_x;

				// look for a pattern in the same row
				if (position / _Columns != search_position / _Columns)
					break;

				// get color of search position
				if (search_position < _Columns * _Rows)
					matching_color = _Colors[search_position];

				// stop looking if the colors don't match
				if (matching_color != color)
					break;
				else
					++pattern_count_x;
			}

			// look vertically

			while (true)
			{
				int search_position = position + pattern_count_y * _Columns;

				// get color of search position

				// make sure search is not out of bounds
				if (search_position < _Columns * _Rows)
					matching_color = _Colors[search_position];
				else
					break;
				// stop looking if the colors don't match
				if (matching_color != color)
					break;
				else
					++pattern_count_y;
			}

			// check if a pattern of at least 3 horizontal matching colors
			if (pattern_count_x >= 3)
			{
				for (int pattern = 0; pattern < pattern_count_x; ++pattern)
				{
					int pattern_position = position + pattern;
					_ColorPatterns.at(pattern_position) = true;
				}
			}
			// check if a pattern of at least 3 vertical matching colors
			if (pattern_count_y >= 3)
			{
				for (int pattern = 0; pattern < pattern_count_y; ++pattern)
				{
					int pattern_position = position + pattern * _Columns;
					_ColorPatterns.at(pattern_position) = true;
				}
			}
			if (pattern_count_x >= 4)
			{
				// Spawn horizontal bomb on second position
				// insert bomb position and type to map
				_BombPositions.insert(std::pair<int, int>(position + 2, 6));
			}

			if (pattern_count_y >= 4)
			{
				// Spawn vertical bomb on second position
				// insert bomb position and type to map
				_BombPositions.insert(std::pair<int, int>(position + 2 * _Columns, 7));
			}
		}
	}
}

void BoardLogic::DetectGemSquareColorPatterns()
{
	for (int y = 0; y < _Rows - 1; ++y)
	{
		for (int x = 0; x < _Columns - 1; ++x)
		{
			// get positions of 4 adjacent cells
			int position1 = y * _Columns + x;
			int position2 = y * _Columns + x + 1;
			int position3 = (y + 1) * _Columns + x;
			int position4 = (y + 1) * _Columns + x + 1;

			// make sure that cells are in same row 
			if (position1 / _Columns != position2 / _Columns && position3 / _Columns != position4 / _Columns)
				continue;

			// check if 4 cells forming square have same color
			if (_Colors[position1] == _Colors[position2] && _Colors[position2] == _Colors[position3] && _Colors[position3] == _Colors[position4])
			{
				_ColorPatterns.at(position1) = true;
				_ColorPatterns.at(position2) = true;
				_ColorPatterns.at(position3) = true;
				_ColorPatterns.at(position4) = true;

				int arr[4]{ position1, position2, position3, position4 };

				// bomb should be spawned in one of 4 positions
				// insert bomb position and type radial
				_BombPositions.insert(std::pair<int, int>(arr[uint_dist(_RandomNumberGenerator) % 4], 8));  //push_back(arr[uint_dist(_RandomNumberGenerator) % 4]);
			}
		}
	}
}

bool BoardLogic::HasPatterns()
{
	for (auto i : _ColorPatterns)
	{
		// beginning of color patterns found
		if (i == true)
			return true;
	}

	return false;
}

void BoardLogic::RemovePattern()
{
	for (int i = 0; i < _Rows * _Columns; ++i)
	{
		if (_ColorPatterns.at(i) == true)
		{
			_Colors[i] = -1;

			// invoke removal event
			Color_Removed(i % _Columns, i / _Columns);

			//track how many empty cell there are after removal
			++_EmptyCellsNum;
		}
	}

	//reset color patterns
	for (auto i : _ColorPatterns)
	{
		i = false;
	}
}

const int BoardLogic::GetGemColor(int column, int row)
{
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns)
	{
		return _Colors[row * _Columns + column];
	}
	else
	{
		return 0;
	}
}

void BoardLogic::SpawnColors()
{
	// iterate backwards and look for empty cells
	for (int y = _Rows - 1; y >= 0; --y)
	{
		for (int x = _Columns - 1; x >= 0; --x)
		{
			int position = y * _Columns + x;

			// check for empty cell here
			if (_Colors[position] == -1)
			{
				// get position of color above
				int swap_position = position - _Columns;

				// if the position is not out of bound move the color down one cell
				if (swap_position >= 0)
				{
					_Colors[position] = _Colors[swap_position];
					_Colors[swap_position] = -1;

					// check if no empty cell here
					if (_Colors[position] != -1)
					{
						// Call spawn color event
						Color_Spawned(x, y - 1);
					}
				}
				else
					// the empty cell is in the top row and it gets filled with a new random color
				{
					_Colors[position] = GetRandomColor();

					// Call spawn color event
					Color_Added(x, y);

					// one empty cell was moved to the top and has been removed
					--_EmptyCellsNum;
				}

			}
		}
	}
}

void BoardLogic::SpawnBomb(int cell_x, int cell_y)
{
	while (!_BombPositions.empty())
	{
		// get bomb position and type
		auto it = _BombPositions.begin();
		_Colors[it->first] = it->second;
		Bomb_Added(it->first % _Columns, it->first / _Columns);
		_BombPositions.erase(it);
		--_EmptyCellsNum;
	}
}

bool BoardLogic::IsPartOfSamePattern(int cell_x, int cell_y)
{
	// look in all directions to find pattern 

	int source_color = _Colors[cell_y * _Columns + cell_x];

	// look right
	if (cell_x < (_Columns - 2))
	{
		if (source_color == _Colors[cell_y * _Columns + cell_x + 1] && source_color == _Colors[cell_y * _Columns + cell_x + 2])
			return true;
	}

	// look left
	if (cell_x > 1)
	{
		if (source_color == _Colors[cell_y * _Columns + cell_x - 1] && source_color == _Colors[cell_y * _Columns + cell_x - 2])
			return true;
	}

	// look left and right 1 cell
	if (cell_x > 0 && cell_x < (_Columns - 1))
	{
		if (source_color == _Colors[cell_y * _Columns + cell_x - 1] && source_color == _Colors[cell_y * _Columns + cell_x + 1])
			return true;
	}

	// look down
	if (cell_y < (_Rows - 2))
	{
		if (source_color == _Colors[(cell_y + 1) * _Columns + cell_x] && source_color == _Colors[(cell_y + 2) * _Columns + cell_x])
			return true;
	}

	// look up
	if (cell_y > 1)
	{
		if (source_color == _Colors[(cell_y - 1) * _Columns + cell_x] && source_color == _Colors[(cell_y - 2) * _Columns + cell_x])
			return true;
	}

	// look up and down 1 cell
	if (cell_y > 0 && cell_y < (_Rows - 1))
	{
		if (source_color == _Colors[(cell_y - 1) * _Columns + cell_x] && source_color == _Colors[(cell_y + 1) * _Columns + cell_x])
			return true;
	}

	// Cases of square
	if (cell_y > 0 && cell_x > 0)
	{
		// look up, left and diagonally left one cell
		if (source_color == _Colors[(cell_y - 1) * _Columns + cell_x] && source_color == _Colors[cell_y * _Columns + cell_x - 1] && source_color == _Colors[(cell_y - 1) * _Columns + cell_x - 1])
			return true;
	}

	if (cell_y > 0 && cell_x < (_Columns - 1))
	{
		// look up, right and diagonally right one cell
		if (source_color == _Colors[(cell_y - 1) * _Columns + cell_x] && source_color == _Colors[cell_y * _Columns + cell_x + 1] && source_color == _Colors[(cell_y - 1) * _Columns + cell_x + 1])
			return true;
	}

	if (cell_y < (_Rows - 1) && cell_x > 0)
	{
		// look down, left and diagonally left one cell
		if (source_color == _Colors[(cell_y + 1) * _Columns + cell_x] && source_color == _Colors[cell_y * _Columns + cell_x - 1] && source_color == _Colors[(cell_y + 1) * _Columns + cell_x - 1])
			return true;
	}

	if (cell_y < (_Rows - 1) && cell_x < (_Columns - 1))
	{
		// look down, right and diagonally right one cell
		if (source_color == _Colors[(cell_y + 1) * _Columns + cell_x] && source_color == _Colors[cell_y * _Columns + cell_x + 1] && source_color == _Colors[(cell_y + 1) * _Columns + cell_x + 1])
			return true;
	}

	return false;
}

void BoardLogic::ColorAddedHandler(std::function<void(int, int)> handlerFunction)
{
	Color_Added = handlerFunction;
}

void BoardLogic::ColorRemovedHandler(std::function<void(int, int)> handlerFunction)
{
	Color_Removed = handlerFunction;
}

void BoardLogic::ColorSpawnedHandler(std::function<void(int, int)> handlerFunction)
{
	Color_Spawned = handlerFunction;
}

void BoardLogic::BombAddedHandler(std::function<void(int, int)> handlerFunction)
{
	Bomb_Added = handlerFunction;
}

const int& BoardLogic::GetColumns()
{
	return _Columns;
}

const int& BoardLogic::GetRows()
{
	return _Rows;
}

const int& BoardLogic::GetEmptyCells()
{
	return _EmptyCellsNum;
}
