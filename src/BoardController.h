#pragma once

#include "BoardLogic.h"
#include "DrawBoard.h"

class BoardLogic;
class DrawBoard;

// Controls DrawBoard and BoardLogic behavior
class BoardController
{
public:
    enum class BoardState
    {
        Default,
        GemsSwapState,
        GemsSelectState,
        PatternDetectState,
        PatternsRemovedState,
        GemClearState,
        GemsSpawnState
    };

    BoardController(int board_col_size, int board_row_size, int cell_size);

    // update board state
    void Update();

    // get pointer to DrawBoard
    std::shared_ptr<DrawBoard> GetDrawBoard() const { return b_drawBoard; }

private:
    // swap gems if pattern is found
    void TrySwapGems();

    // detect color pattern
    void FindPattern();

    // remove color pattern
    void RemovePattern();

    // clear removed gems
    void ClearRemovedGems();

    // Adds new gems
    void SpawnGems();

    // callback function for two gems have been selected
    constexpr void OnGemSelect(int source_x, int source_y, int target_x, int target_y);

    // callback function for new added color
    void OnColorAdd(int column, int row);

    // callback function for color removed from the board
    void OnColorRemove(int column, int row);

    // callback function for color spawned
    void OnColorSpawn(int column, int row);

    // callback function for bomb spawn
    void OnBombSpawn(int column, int row);

    // callback function for bomb detonate
    bool OnBombSelect(int column, int row);

private:
    // store selected gems
    int b_sourceCellX;
    int b_sourceCellY;
    int b_targetCellX;
    int b_targetCellY;

    BoardState b_boardState;
    std::unique_ptr<BoardLogic> b_boardLogic;
    std::shared_ptr<DrawBoard> b_drawBoard;
};
