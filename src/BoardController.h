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
        GemsSwapFailed,
        GemsSelectState,
        PatternDetectState,
        PatternsRemovedState,
        GemClearState,
        GemsSpawnState
    };

    BoardController(unsigned render_window_w, unsigned render_window_h);

    // update board state
    void Update();

    // get loose condition status
    [[nodiscard]] bool GetGameOverStatus() const { return b_isGameOver; }

    // get win condition status
    [[nodiscard]] bool GetWinStatus() const { return b_isWin; }

    // get pointer to DrawBoard
    [[nodiscard]] std::shared_ptr<DrawBoard> GetDrawBoard() const { return b_drawBoard; }

private:
    // swap gems if pattern is found
    void TrySwapGems();

    // return gems to previous position
    void RevertGemSwap();

    // detect color pattern
    void FindPattern();

    // remove color pattern
    void RemovePattern();

    // clear removed gems
    void ClearRemovedGems();

    // Adds new gems
    void SpawnGems();

    // callback function for two gems have been selected
    constexpr void OnGemSelect(unsigned source_x, unsigned source_y, unsigned target_x, unsigned target_y);

    // callback function for new added color
    void OnColorAdd(unsigned column, unsigned row);

    // callback function for color removed from the board
    void OnColorRemove(unsigned column, unsigned row);

    // callback function for color spawned
    void OnColorSpawn(unsigned column, unsigned row);

    // callback function for bomb spawn
    void OnBombSpawn(unsigned column, unsigned row);

    // callback function for bomb detonate
    bool OnBombSelect(unsigned column, unsigned row);

    // callback function for move count update
    void OnMoveUpdate();

    // callback function for objectives update
    void OnObjectiveHit();

private:
    // store selected gems
    unsigned b_sourceCellX;
    unsigned b_sourceCellY;
    unsigned b_targetCellX;
    unsigned b_targetCellY;

    bool b_isGameOver;
    bool b_isWin;

    BoardState b_boardState;
    std::unique_ptr<BoardLogic> b_boardLogic;
    std::shared_ptr<DrawBoard> b_drawBoard;
};
