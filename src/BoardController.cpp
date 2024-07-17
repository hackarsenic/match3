#include "GameConfig.h"
#include "BoardLogic.h"
#include "DrawBoard.h"

#include "BoardController.h"
#include "Gem.h"

BoardController::BoardController(int render_window_w, int render_window_h)
    : b_sourceCellX(0),
      b_sourceCellY(0),
      b_targetCellX(0),
      b_targetCellY(0),
      b_isGameOver(false),
      b_isWin(false),
      b_boardState(BoardState::Default),
      b_boardLogic(std::make_unique<BoardLogic>()),
      b_drawBoard(std::make_unique<DrawBoard>(render_window_w, render_window_h))
{
    // set view properties
    b_drawBoard->SetPosition(GameConfig::GetInstance().GetCellSize(), GameConfig::GetInstance().GetCellSize() * 3);

    // register callbacks
    b_drawBoard->GemsSelectedHandler([this](int sourceX, int sourceY, int targetX, int targetY) { return OnGemSelect(sourceX, sourceY, targetX, targetY); });
    b_drawBoard->BombSelectedHandler([this](int column, int row) { return OnBombSelect(column, row); });
    b_boardLogic->ColorAddedHandler([this](int column, int row) { return OnColorAdd(column, row); });
    b_boardLogic->ColorRemovedHandler([this](int column, int row) { return OnColorRemove(column, row); });
    b_boardLogic->ColorSpawnedHandler([this](int column, int row) { return OnColorSpawn(column, row); });
    b_boardLogic->BombAddedHandler([this](int column, int row) { return OnBombSpawn(column, row); });
    b_boardLogic->MoveCountHandler([this]() { return OnMoveUpdate(); });
    b_boardLogic->ObjectiveUpdateHandler( [this]() { return OnObjectiveHit(); });

    // generate the board gems
    b_boardLogic->Generate();

    // board dimensions
    const int columns = b_boardLogic->GetColumns();
    const int rows = b_boardLogic->GetRows();

    // get gems to draw on board
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            int color = b_boardLogic->GetGemColor(x, y);
            b_drawBoard->Add(x, y, Gem::Color(color));
        }
    }
}

void BoardController::Update()
{
    // after every update check if gems were swapped.
    // If gems were swapped check if patterns were removed
    if (!b_drawBoard->GetAnimationState()) {
        switch (b_boardState) {
            case BoardState::GemsSelectState:
                // try to swap selected gems
                TrySwapGems();
                break;
            case BoardState::GemsSwapState:
                // find pattern
                FindPattern();
                break;
            case BoardState::GemsSwapFailed:
                // return swapped gems to original position
                RevertGemSwap();
                break;
            case BoardState::PatternDetectState:
                // remove all patterns
                RemovePattern();
                break;
            case BoardState::GemClearState:
                // clear pending removal gems
                ClearRemovedGems();
                break;
            case BoardState::PatternsRemovedState:
                // spawn gems one row at a time until no empty cells left
                SpawnGems();
                break;
            case BoardState::GemsSpawnState:
                // find pattern
                FindPattern();
                break;
            default:
                break;
        }
    }
}

constexpr void BoardController::OnGemSelect(int source_x, int source_y, int target_x, int target_y)
{
    // store coordinates of source and target cells
    b_sourceCellX = source_x;
    b_sourceCellY = source_y;
    b_targetCellX = target_x;
    b_targetCellY = target_y;

    // gems selected
    b_boardState = BoardState::GemsSelectState;
}

bool BoardController::OnBombSelect(int column, int row)
{
    // Try to detonate bomb if bomb is selected
    if (b_boardLogic->DetonateBomb(column, row)) {
        b_boardState = BoardState::PatternDetectState;
        return true;
    }

    return false;
}

void BoardController::TrySwapGems()
{
    // check if the attempted swap leads to a pattern
    const bool is_swap_valid = b_boardLogic->SwapGemColors(b_sourceCellX, b_sourceCellY, b_targetCellX, b_targetCellY);

    // get gems pending to swap
    const std::shared_ptr<Gem> &first_gem = b_drawBoard->GetGem(b_sourceCellX, b_sourceCellY);
    const std::shared_ptr<Gem> &second_gem = b_drawBoard->GetGem(b_targetCellX, b_targetCellY);

    if (is_swap_valid) {
        // swap gems in draw board
        b_drawBoard->Move(first_gem, b_targetCellX, b_targetCellY);
        b_drawBoard->Move(second_gem, b_sourceCellX, b_sourceCellY);

        // swapped successfully
        b_boardState = BoardState::GemsSwapState;
    } else {
        b_drawBoard->Move(first_gem, b_targetCellX, b_targetCellY);
        b_drawBoard->Move(second_gem, b_sourceCellX, b_sourceCellY);

        std::swap(b_sourceCellX, b_targetCellX);
        std::swap(b_sourceCellY, b_targetCellY);

        // needs reverting
        b_boardState = BoardState::GemsSwapFailed;
    }
}

void BoardController::RevertGemSwap()
{
    // get gems pending to swap
    const std::shared_ptr<Gem> &first_gem = b_drawBoard->GetGem(b_sourceCellX, b_sourceCellY);
    const std::shared_ptr<Gem> &second_gem = b_drawBoard->GetGem(b_targetCellX, b_targetCellY);

    // swap gems in draw board
    b_drawBoard->Move(first_gem, b_targetCellX, b_targetCellY);
    b_drawBoard->Move(second_gem, b_sourceCellX, b_sourceCellY);

    b_boardState = BoardState::Default;
}

void BoardController::FindPattern()
{
    // find matching color pattern
    b_boardLogic->DetectGemColorPatterns();
    b_boardLogic->DetectGemSquareColorPatterns();

    if (b_boardLogic->HasPatterns())
        b_boardState = BoardState::PatternDetectState;
}

void BoardController::RemovePattern()
{
    b_boardLogic->RemovePattern();
    b_boardState = BoardState::GemClearState;

    const auto move_num = b_boardLogic->GetMoveCount();
    if (move_num < 1)
        b_isGameOver = true;
}

void BoardController::ClearRemovedGems()
{
    b_drawBoard->ClearPendingRemovalGems();
    b_boardState = BoardState::PatternsRemovedState;
}

void BoardController::SpawnGems()
{
    // spawn new colors until no empty cells left
    if (b_boardLogic->GetEmptyCells() > 0) {
        b_boardLogic->SpawnBomb(0, 0);
        b_boardLogic->SpawnColors();
    } else {
        b_boardState = BoardState::GemsSpawnState;
    }
}

void BoardController::OnColorAdd(int column, int row)
{
    // retrieve color
    const int color = b_boardLogic->GetGemColor(column, row);
    // draw new gem on board
    b_drawBoard->Add(column, row, Gem::Color(color));
}

void BoardController::OnColorRemove(int column, int row)
{
    b_drawBoard->MarkRemoved(column, row);
}

void BoardController::OnColorSpawn(int column, int row)
{
    // retrieve gem at position
    const auto &gem = b_drawBoard->GetGem(column, row);
    // move down one row
    b_drawBoard->Move(gem, column, row + 1);
}

void BoardController::OnBombSpawn(int column, int row)
{
    // retrieve color
    const int color = b_boardLogic->GetGemColor(column, row);
    // draw new gem on board
    b_drawBoard->Add(column, row, Gem::Color(color));
}

void BoardController::OnMoveUpdate()
{
    // retrieve move count
    const int move_num = b_boardLogic->GetMoveCount();
    // draw move counter on board
    b_drawBoard->UpdateMoveCount(move_num);
}

void BoardController::OnObjectiveHit()
{
    // retrieve objectives
    const auto &objectives = b_boardLogic->GetObjectives();
    // update objective on board
    b_drawBoard->UpdateObjectives(objectives);

    const bool is_complete = std::all_of(objectives.begin(), objectives.end(), [](const auto& p) { return p.second <= 0; });

    if (is_complete)
        b_isWin = true;
}
