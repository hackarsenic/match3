#include "GameConfig.h"
#include "BoardLogic.h"
#include "Gem.h"

BoardLogic::BoardLogic()
    : b_columns(GameConfig::GetInstance().GetColSize()),
      b_rows(GameConfig::GetInstance().GetRowSize()),
      b_moveCount(GameConfig::GetInstance().GetMoveCount()),
      b_objectives(GameConfig::GetInstance().GetObjectives()),
      b_emptyCellsNum(0)
{
    // resize vectors according to board size
    b_colors.resize(b_columns * b_rows, -1);
    b_colorPatterns.resize(b_columns * b_rows, false);

    // seed with time
    b_randomNumberGenerator.seed(time(nullptr));

    // get the objectives
    for (const auto &p : GameConfig::GetInstance().GetObjectives()) {
        b_objectives.insert({p.first, p.second});
    }
}

bool BoardLogic::SwapGemColors(unsigned source_x, unsigned source_y, unsigned target_x, unsigned target_y)
{
    const unsigned source_position = source_y * b_columns + source_x;
    const unsigned target_position = target_y * b_columns + target_x;

    // try to swap colors - check for pattern
    const int source_color = b_colors[source_y * b_columns + source_x];
    const int target_color = b_colors[target_y * b_columns + target_x];

    // swap
    b_colors[source_position] = target_color;
    b_colors[target_position] = source_color;

    // check if any of selected colors is now part of a pattern
    if (IsPartOfSamePattern(source_x, source_y) || IsPartOfSamePattern(target_x, target_y)) {
        --b_moveCount;
        b_MoveCountUpdated();
        return true;
    } else {
        // reverse swap
        b_colors[source_position] = source_color;
        b_colors[target_position] = target_color;

        return false;
    }
}

bool BoardLogic::DetonateBomb(int cell_x, int cell_y)
{
    // Square bomb detonation case
    if (b_colors[cell_y * b_columns + cell_x] == static_cast<int>(Gem::Color::Bomb)) {
        if (b_colors[cell_y * b_columns + cell_x] != -1)
            b_ColorRemoved(cell_x, cell_y);
        b_colors[cell_y * b_columns + cell_x] = -1;
        ++b_emptyCellsNum;

        const auto is_valid = [this](int row, int col) -> bool { return (row >= 0) && (row < b_rows) && (col >= 0) && (col < b_columns); };

        // checks for edge cases
        if (is_valid(cell_y - 1, cell_x - 1)) {
            const size_t gem_idx = (cell_y - 1) * b_columns + cell_x - 1;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x - 1, cell_y - 1);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x - 1, cell_y - 1);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }

        if (is_valid(cell_y - 1, cell_x)) {
            const size_t gem_idx = (cell_y - 1) * b_columns + cell_x;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x, cell_y - 1);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x, cell_y - 1);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }

        if (is_valid(cell_y - 1, cell_x + 1)) {
            const size_t gem_idx = (cell_y - 1) * b_columns + cell_x + 1;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x + 1, cell_y - 1);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x + 1, cell_y - 1);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }

        if (is_valid(cell_y, cell_x - 1)) {
            const size_t gem_idx = cell_y * b_columns + cell_x - 1;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x - 1, cell_y);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x - 1, cell_y);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }

        if (is_valid(cell_y, cell_x + 1)) {
            const size_t gem_idx = cell_y * b_columns + cell_x + 1;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x + 1, cell_y);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x + 1, cell_y);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }

        if (is_valid(cell_y + 1, cell_x - 1)) {
            const size_t gem_idx = (cell_y + 1) * b_columns + cell_x - 1;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x - 1, cell_y + 1);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x - 1, cell_y + 1);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }

        if (is_valid(cell_y + 1, cell_x)) {
            const size_t gem_idx = (cell_y + 1) * b_columns + cell_x;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x, cell_y + 1);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x, cell_y + 1);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }

        if (is_valid(cell_y + 1, cell_x + 1)) {
            const size_t gem_idx = (cell_y + 1) * b_columns + cell_x + 1;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x + 1, cell_y + 1);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x + 1, cell_y + 1);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }
        --b_moveCount;
        b_MoveCountUpdated();

        return true;
    }

    // detonate vertical bomb
    if (b_colors[cell_y * b_columns + cell_x] == static_cast<int>(Gem::Color::Bomb_V)) {
        if (b_colors[cell_y * b_columns + cell_x] != -1)
            b_ColorRemoved(cell_x, cell_y);
        b_colors[cell_y * b_columns + cell_x] = -1;
        ++b_emptyCellsNum;

        for (int i = 0; i < b_rows; ++i) {
            const size_t gem_idx = i * b_columns + cell_x;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > static_cast<int>(Gem::Color::COUNT) && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(cell_x, i);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(cell_x, i);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }
        --b_moveCount;
        b_MoveCountUpdated();

        return true;
    }

    // detonate horizontal bomb
    if (b_colors[cell_y * b_columns + cell_x] == static_cast<int>(Gem::Color::Bomb_H)) {
        if (b_colors[cell_y * b_columns + cell_x] != -1)
            b_ColorRemoved(cell_x, cell_y);
        b_colors[cell_y * b_columns + cell_x] = -1;
        ++b_emptyCellsNum;

        for (int i = 0; i < b_columns; ++i) {
            const size_t gem_idx = i + cell_y * b_columns;
            DecreaseNCheckObjectives(b_colors[gem_idx]);
            if (b_colors[gem_idx] > 5 && b_colors[gem_idx] < 9) {
                ++b_moveCount;
                DetonateBomb(i, cell_y);
            }
            if (b_colors[gem_idx] != -1) {
                b_ColorRemoved(i, cell_y);
                b_colors[gem_idx] = -1;
                ++b_emptyCellsNum;
            }
        }
        --b_moveCount;
        b_MoveCountUpdated();

        return true;
    }

    return false;
}

int BoardLogic::GetRandomColor()
{
    return b_unifINTDist(b_randomNumberGenerator) % static_cast<int>(Gem::Color::COUNT);
}

void BoardLogic::Generate()
{
    // check that the two pieces to the left or two pieces down are not the same color as the gem added
    for (size_t y = 0; y < b_rows; ++y) {
        for (size_t x = 0; x < b_columns; ++x) {
            const auto position = y * b_columns + x;
            int new_color = 0;

            do {
                new_color = GetRandomColor();
            } while (((x > 0 && y > 0) && b_colors[(x - 1) + ((y - 1) * b_columns)] == new_color && b_colors[(x - 1) + (y * b_columns)] == new_color && b_colors[x + ((y - 1) * b_columns)] == new_color) ||
                     (x > 1 && b_colors[(x - 1) + (y * b_columns)] == new_color && b_colors[(x - 2) + (y * b_columns)] == new_color) ||
                     (y > 1 && b_colors[x + ((y - 1) * b_columns)] == new_color && b_colors[x + ((y - 2) * b_columns)] == new_color) ||
                     ((x > 1 && y > 1) && b_colors[x + (y - 1) * b_columns] == new_color));
            b_colors[position] = new_color;
        }
    }
    b_emptyCellsNum = 0;
}

void BoardLogic::DetectGemColorPatterns()
{
    // scan board rows to find horizontal color pattern
    for (int y = 0; y < b_rows; ++y) {
        for (int x = 0; x < b_columns; ++x) {
            const int position = y * b_columns + x;
            const int color = b_colors[position];

            // look in this row for a pattern
            int pattern_count_x = 1;
            int pattern_count_y = 1;

            // look horizontally
            int matching_color = color;

            while (true) {
                const int search_position = position + pattern_count_x;

                // look for a pattern in the same row
                if (position / b_columns != search_position / b_columns)
                    break;

                // get color of search position
                if (search_position < b_columns * b_rows)
                    matching_color = b_colors[search_position];

                // stop looking if the colors don't match
                if (matching_color != color)
                    break;
                else
                    ++pattern_count_x;
            }

            // look vertically

            while (true) {
                int search_position = position + pattern_count_y * b_columns;

                // get color of search position

                // make sure search is not out of bounds
                if (search_position < b_columns * b_rows)
                    matching_color = b_colors[search_position];
                else
                    break;
                // stop looking if the colors don't match
                if (matching_color != color)
                    break;
                else
                    ++pattern_count_y;
            }

            // check if a pattern of at least 3 horizontal matching colors
            if (pattern_count_x >= 3) {
                for (int pattern = 0; pattern < pattern_count_x; ++pattern) {
                    int pattern_position = position + pattern;
                    b_colorPatterns.at(pattern_position) = true;
                }
            }
            // check if a pattern of at least 3 vertical matching colors
            if (pattern_count_y >= 3) {
                for (int pattern = 0; pattern < pattern_count_y; ++pattern) {
                    int pattern_position = position + pattern * b_columns;
                    b_colorPatterns.at(pattern_position) = true;
                }
            }
            if (pattern_count_x >= 4) {
                // Spawn horizontal bomb on second position
                // insert bomb position and type to map
                b_bombPositions.insert(std::pair<int, int>(position + 2, 6));
            }

            if (pattern_count_y >= 4) {
                // Spawn vertical bomb on second position
                // insert bomb position and type to map
                b_bombPositions.insert(std::pair<int, int>(position + 2 * b_columns, 7));
            }
        }
    }
}

void BoardLogic::DetectGemSquareColorPatterns()
{
    for (int y = 0; y < b_rows - 1; ++y) {
        for (int x = 0; x < b_columns - 1; ++x) {
            // get positions of 4 adjacent cells
            const int position1 = y * b_columns + x;
            const int position2 = y * b_columns + x + 1;
            const int position3 = (y + 1) * b_columns + x;
            const int position4 = (y + 1) * b_columns + x + 1;

            // make sure that cells are in same row
            if (position1 / b_columns != position2 / b_columns && position3 / b_columns != position4 / b_columns)
                continue;

            // check if 4 cells forming square have same color
            if (b_colors[position1] == b_colors[position2] && b_colors[position2] == b_colors[position3] && b_colors[position3] == b_colors[position4]) {
                b_colorPatterns.at(position1) = true;
                b_colorPatterns.at(position2) = true;
                b_colorPatterns.at(position3) = true;
                b_colorPatterns.at(position4) = true;

                int arr[4] { position1, position2, position3, position4 };

                // bomb should be spawned in one of 4 positions
                // insert bomb position and type radial
                b_bombPositions.insert(std::pair<int, int>(arr[b_unifINTDist(b_randomNumberGenerator) % 4], 8));
            }
        }
    }
}

bool BoardLogic::HasPatterns() const
{
    for (auto i : b_colorPatterns) {
        // beginning of color patterns found
        if (i)
            return true;
    }

    return false;
}

void BoardLogic::RemovePattern()
{
    for (unsigned i = 0; i < b_rows * b_columns; ++i) {
        if (b_colorPatterns.at(i) == true) {
            DecreaseNCheckObjectives(b_colors[i]);
            b_colors[i] = -1;

            // invoke removal event
            b_ColorRemoved(i % b_columns, i / b_columns);

            // track how many empty cell there are after removal
            ++b_emptyCellsNum;
        }
    }

    // reset color patterns
    for (auto i : b_colorPatterns) {
        i = false;
    }
}

void BoardLogic::SpawnColors()
{
    // iterate backwards and look for empty cells
    for (int y = b_rows - 1; y >= 0; --y) {
        for (int x = b_columns - 1; x >= 0; --x) {
            const int position = y * b_columns + x;

            // check for empty cell here
            if (b_colors[position] == -1) {
                // get position of color above
                int swap_position = position - b_columns;

                // if the position is not out of bound move the color down one cell
                if (swap_position >= 0) {
                    b_colors[position] = b_colors[swap_position];
                    b_colors[swap_position] = -1;

                    // check if no empty cell here
                    if (b_colors[position] != -1) {
                        // Call spawn color event
                        b_ColorSpawned(x, y - 1);
                    }
                } else
                // the empty cell is in the top row and it gets filled with a new random color
                {
                    b_colors[position] = GetRandomColor();

                    // Call spawn color event
                    b_ColorAdded(x, y);

                    // one empty cell was moved to the top and has been removed
                    --b_emptyCellsNum;
                }
            }
        }
    }
}

void BoardLogic::SpawnBomb(int cell_x, int cell_y)
{
    while (!b_bombPositions.empty()) {
        // get bomb position and type
        const auto it = b_bombPositions.cbegin();
        b_colors[it->first] = it->second;
        b_BombAdded(it->first % b_columns, it->first / b_columns);
        b_bombPositions.erase(it);
        --b_emptyCellsNum;
    }
}

bool BoardLogic::IsPartOfSamePattern(unsigned cell_x, unsigned cell_y) const
{
    // look in all directions to find pattern

    const int source_color = b_colors[cell_y * b_columns + cell_x];

    // prevent bomb pattern detection
    if (source_color > static_cast<int>(Gem::Color::COUNT))
        return false;

    // look right
    if (cell_x < (b_columns - 2)) {
        if (source_color == b_colors[cell_y * b_columns + cell_x + 1] && source_color == b_colors[cell_y * b_columns + cell_x + 2])
            return true;
    }

    // look left
    if (cell_x > 1) {
        if (source_color == b_colors[cell_y * b_columns + cell_x - 1] && source_color == b_colors[cell_y * b_columns + cell_x - 2])
            return true;
    }

    // look left and right 1 cell
    if (cell_x > 0 && cell_x < (b_columns - 1)) {
        if (source_color == b_colors[cell_y * b_columns + cell_x - 1] && source_color == b_colors[cell_y * b_columns + cell_x + 1])
            return true;
    }

    // look down
    if (cell_y < (b_rows - 2)) {
        if (source_color == b_colors[(cell_y + 1) * b_columns + cell_x] && source_color == b_colors[(cell_y + 2) * b_columns + cell_x])
            return true;
    }

    // look up
    if (cell_y > 1) {
        if (source_color == b_colors[(cell_y - 1) * b_columns + cell_x] && source_color == b_colors[(cell_y - 2) * b_columns + cell_x])
            return true;
    }

    // look up and down 1 cell
    if (cell_y > 0 && cell_y < (b_rows - 1)) {
        if (source_color == b_colors[(cell_y - 1) * b_columns + cell_x] && source_color == b_colors[(cell_y + 1) * b_columns + cell_x])
            return true;
    }

    // Cases of square
    if (cell_y > 0 && cell_x > 0) {
        // look up, left and diagonally left one cell
        if (source_color == b_colors[(cell_y - 1) * b_columns + cell_x] && source_color == b_colors[cell_y * b_columns + cell_x - 1] && source_color == b_colors[(cell_y - 1) * b_columns + cell_x - 1])
            return true;
    }

    if (cell_y > 0 && cell_x < (b_columns - 1)) {
        // look up, right and diagonally right one cell
        if (source_color == b_colors[(cell_y - 1) * b_columns + cell_x] && source_color == b_colors[cell_y * b_columns + cell_x + 1] && source_color == b_colors[(cell_y - 1) * b_columns + cell_x + 1])
            return true;
    }

    if (cell_y < (b_rows - 1) && cell_x > 0) {
        // look down, left and diagonally left one cell
        if (source_color == b_colors[(cell_y + 1) * b_columns + cell_x] && source_color == b_colors[cell_y * b_columns + cell_x - 1] && source_color == b_colors[(cell_y + 1) * b_columns + cell_x - 1])
            return true;
    }

    if (cell_y < (b_rows - 1) && cell_x < (b_columns - 1)) {
        // look down, right and diagonally right one cell
        if (source_color == b_colors[(cell_y + 1) * b_columns + cell_x] && source_color == b_colors[cell_y * b_columns + cell_x + 1] && source_color == b_colors[(cell_y + 1) * b_columns + cell_x + 1])
            return true;
    }

    return false;
}

void BoardLogic::DecreaseNCheckObjectives(int gem)
{
    if (const auto &o = b_objectives.find(gem); o != b_objectives.end()) {
        --o->second;
        if (o->second < 0)
            o->second = 0;
    }

    b_ObjectivesUpdated();
}

void BoardLogic::ColorAddedHandler(std::function<void(int, int)> handler_function)
{
    b_ColorAdded = std::move(handler_function);
}

void BoardLogic::ColorRemovedHandler(std::function<void(int, int)> handler_function)
{
    b_ColorRemoved = std::move(handler_function);
}

void BoardLogic::ColorSpawnedHandler(std::function<void(int, int)> handler_function)
{
    b_ColorSpawned = std::move(handler_function);
}

void BoardLogic::BombAddedHandler(std::function<void(int, int)> handler_function)
{
    b_BombAdded = std::move(handler_function);
}

void BoardLogic::MoveCountHandler(std::function<void()> handler_function)
{
    b_MoveCountUpdated = std::move(handler_function);
}

void BoardLogic::ObjectiveUpdateHandler(std::function<void()> handler_function)
{
    b_ObjectivesUpdated = std::move(handler_function);
}

unsigned BoardLogic::GetColumns() const
{
    return b_columns;
}

unsigned BoardLogic::GetRows() const
{
    return b_rows;
}

int BoardLogic::GetGemColor(int column, int row) const
{
    if (row >= 0 && row < b_rows && column >= 0 && column < b_columns) {
        return b_colors[row * b_columns + column];
    } else {
        return 0;
    }
}

int BoardLogic::GetEmptyCells() const
{
    return b_emptyCellsNum;
}

int BoardLogic::GetMoveCount() const
{
    return b_moveCount;
}

const std::map<int, int> &BoardLogic::GetObjectives() const
{
    return b_objectives;
}
