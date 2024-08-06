#pragma once

#include <random>
#include <vector>
#include <map>
#include <functional>

// Board's work logic goes here
class BoardLogic
{
public:
    BoardLogic();

    // generate random colors
    void Generate();

    // swap source cell and target cell
    bool SwapGemColors(unsigned source_x, unsigned sourceY, unsigned targetX, unsigned targetY);

    // Blows up the selected bomb
    bool DetonateBomb(int cell_x, int cell_y);

    // detects horizonal and vertical matching color patterns
    void DetectGemColorPatterns();

    // detects square matching color patterns
    void DetectGemSquareColorPatterns();

    // check if current board state contains patterns
    [[nodiscard]] bool HasPatterns() const;

    // remove matching color patterns
    void RemovePattern();

    // adds colors to empty cells
    // empty cells move to top than get filled with colors
    void SpawnColors();

    // add bomb when formation detected
    void SpawnBomb(int cell_x, int cell_y);

    // returns the amount of columns
    [[nodiscard]] unsigned GetColumns() const;
    // returns the amount of rows
    [[nodiscard]] unsigned GetRows() const;
    // returns cell color
    [[nodiscard]] int GetGemColor(int column, int row) const;
    // returns amount of empty cells
    [[nodiscard]] int GetEmptyCells() const;
    // returns remaining move number
    [[nodiscard]] int GetMoveCount() const;
    // returns current state of objectives
    [[nodiscard]] const std::map<int, int> &GetObjectives() const;

    // methods for registering state changes (color changes)
    void ColorAddedHandler(std::function<void(int, int)> handler_function);
    void ColorRemovedHandler(std::function<void(int, int)> handler_function);
    void ColorSpawnedHandler(std::function<void(int, int)> handler_function);
    void BombAddedHandler(std::function<void(int, int)> handler_function);
    void MoveCountHandler(std::function<void()> handler_function);
    void ObjectiveUpdateHandler(std::function<void()> handler_function);

private:
    // generate a random color
    int GetRandomColor();

    // check if selected color is part of patterns
    bool IsPartOfSamePattern(unsigned cell_x, unsigned cell_y) const;

    // check if objective found
    void DecreaseNCheckObjectives(int gem);

private:
    // board size
    unsigned b_columns;
    unsigned b_rows;

    // win conditions
    int b_moveCount;
    std::map<int, int> b_objectives;

    // vector holding color info of all cells
    std::vector<int> b_colors;

    // map holding bomb positions and type needed to spawn
    std::map<int, int> b_bombPositions;

    // Number of empty cells
    int b_emptyCellsNum;

    // a bit array that describes valid color patterns on the board
    std::vector<bool> b_colorPatterns;

    // random number generation
    std::minstd_rand0 b_randomNumberGenerator;

    // uniform distribution for random number generation
    std::uniform_int_distribution<int> b_unifINTDist;

    // handler functions
    std::function<void(unsigned, unsigned)> b_ColorSpawned;
    std::function<void(unsigned, unsigned)> b_ColorAdded;
    std::function<void(unsigned, unsigned)> b_ColorRemoved;
    std::function<void(unsigned, unsigned)> b_BombAdded;
    std::function<void()> b_MoveCountUpdated;
    std::function<void()> b_ObjectivesUpdated;
};
