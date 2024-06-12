#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "Gem.h"

class Gem;

// Visible part of game

class DrawBoard : public sf::Drawable
{
public:
    friend class Animation;

    DrawBoard(int columns, int rows, int cell_size = 110);
    ~DrawBoard() override = default;

    enum class DrawBoardState
    {
        SelectFirstGemState,
        SelectSecondGemState,
        RemoveGemsState
    };

    // position of the board
    void SetPosition(int x, int y);

    // gets gem of selected cell
    std::unique_ptr<Gem> GetGem(int column, int row);

    // check if animation playing
    bool GetAnimationState() const { return d_isAnimating; }

    // adds gem to cell of specified color
    void Add(int column, int row, Gem::Color color);

    // removes gem from cell
    void MarkRemoved(int column, int row);

    // moves gem to target cell
    void Move(std::unique_ptr<Gem> move_gem, int column, int row);

    // method registers gem selection event
    void GemsSelectedHandler(std::function<void(int, int, int, int)> handlerFunction);
    void BombSelectedHandler(std::function<bool(int, int)> handlerFunction);
    void ScaleAnimationHandler(std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> handlerFunction);
    void MoveHorizAnimationHandler(std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> handlerFunction);
    void MoveVertAnimationHandler(std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> handlerFunction);

    // registers mouse events
    void OnMousePress(sf::Event *event);

    // clear gems marked for removal
    void ClearPendingRemovalGems();

private:
    // set board width
    constexpr void SetWidth(int width);

    // set board height
    constexpr void SetHeight(int height);

    // set cell size according to column, row quantity and
    // actual board width height
    constexpr void InitBoardDimensions();

    // Generates score board texture
    void GenerateScoreBoardTexture() const;

    // Generates game board texture
    void GenerateTileBoardTexture() const;

    // Overrides draw funtion of base Drawable
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    // Swaps selected cells
    constexpr void SwapCells(int first_cell, int second_cell);

    // Try blow bomb if found
    bool DetonateBomb(int cell);

private:
    // amount of cells
    int d_columns;
    int d_rows;

    // position and dimensions of the board
    int d_positionX;
    int d_positionY;
    int d_width;
    int d_height;

    // dimensions of the cells
    int d_cellWidth;
    int d_cellHeight;

    // mouse position
    int d_mousePositionX;
    int d_mousePositionY;

    float d_gemPositioningTweakFactor;
    float d_targetGemSize;

    // vector that holds all gems
    std::vector<std::unique_ptr<Gem>> d_gemArray;
    std::vector<std::unique_ptr<Gem>> d_pendingRemovalGemArray;

    // holds the current board state
    DrawBoardState d_state;

    // track selection
    int d_selectedCell;

    // holds info if gem is selected
    bool d_isGemSelected;

    // is animation playing
    bool d_isAnimating;

    // holds score board background texture
    std::unique_ptr<sf::RenderTexture> d_combinedScoreTexture;

    // holds game board background texture
    std::unique_ptr<sf::RenderTexture> d_combinedTileTexture;

    // gameplay handler functions
    std::function<void(int, int, int, int)> d_GemsSelected;
    std::function<bool(int, int)> d_BombSelected;

    // animation handler functions
    std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> d_PlayScaleAnimation;
    std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> d_PlayMoveHorizAnimation;
    std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> d_PlayMoveVertAnimation;

    // resolution of view board will be drawn on
    static int d_ResolutionW;
    static int d_ResolutionH;

    // flag to generate score board only once
    static bool d_scFlag;
};
