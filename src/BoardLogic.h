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
    bool SwapGemColors(int source_x, int sourceY, int targetX, int targetY);

    // Blows up the selected bomb
    bool DetonateBomb(int cell_x, int cell_y);

    // detects horizonal and vertical matching color patterns
    void DetectGemColorPatterns();

    // detects square matching color patterns
    void DetectGemSquareColorPatterns();

    // check if current board state contains patterns
    bool HasPatterns() const;

    // remove matching color patterns
    void RemovePattern();

    // adds colors to empty cells
    // empty cells move to top than get filled with colors
    void SpawnColors();

    // add bomb when formation detected
    void SpawnBomb(int cell_x, int cell_y);

    // returns the amount of columns
    int GetColumns() const;
    // returns the amount of rows
    int GetRows() const;
    // returns cell color
    int GetGemColor(int column, int row) const;
    // returns amount of empty cells
    int GetEmptyCells() const;
    // returns remaining move number
    int GetMoveCount() const;
    // returns current state of objectives
    const std::map<int, int> &GetObjectives() const;

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
    bool IsPartOfSamePattern(int cell_x, int cell_y) const;

    // check if objective found
    void DecreaseNCheckObjectives(int gem);

private:
    // board size
    int b_columns;
    int b_rows;

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
    std::function<void(int, int)> b_ColorSpawned;
    std::function<void(int, int)> b_ColorAdded;
    std::function<void(int, int)> b_ColorRemoved;
    std::function<void(int, int)> b_BombAdded;
    std::function<void()> b_MoveCountUpdated;
    std::function<void()> b_ObjectivesUpdated;
};