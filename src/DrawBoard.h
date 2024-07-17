#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "Gem.h"

class Gem;

// Visible part of game

class DrawBoard : public sf::Drawable
{
public:
    enum class DrawBoardState
    {
        SelectFirstGemState,
        SelectSecondGemState,
        RemoveGemsState
    };

    friend class Animation;

    DrawBoard(int render_window_w, int render_window_h);
    ~DrawBoard() override = default;

    // position of the board
    void SetPosition(int x, int y);

    // gets gem of selected cell
    std::shared_ptr<Gem> GetGem(int column, int row);

    // check if animation playing
    bool GetAnimationState() const { return d_isAnimating; }

    // adds gem to cell of specified color
    void Add(int column, int row, Gem::Color color);

    // removes gem from cell
    void MarkRemoved(int column, int row);

    // moves gem to target cell
    void Move(const std::shared_ptr<Gem> &move_gem, int column, int row);

    // updates move number on scoreboard
    void UpdateMoveCount(int move_count);

    void UpdateObjectives(const std::map<int, int> &objectives);

    // method registers gem selection event
    void GemsSelectedHandler(std::function<void(int, int, int, int)> handlerFunction);
    void BombSelectedHandler(std::function<bool(int, int)> handlerFunction);
    void ScaleAnimationHandler(std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> handlerFunction);
    void MoveHorizAnimationHandler(std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> handlerFunction);
    void MoveVertAnimationHandler(std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> handlerFunction);

    // registers mouse events
    void OnMousePress(sf::Event *event);

    // clear gems marked for removal
    void ClearPendingRemovalGems();

private:
    // set cell size according to column, row quantity and
    // actual board width height
    constexpr void InitBoardDimensions();

    // Generates objectives
    void GenerateObjectives();

    // Generates score board texture
    void GenerateScoreBoardTexture() const;

    // Generates game board texture
    void GenerateTileBoardTexture() const;

    // Overrides draw function of base Drawable
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

    // dimensions of the cells
    int d_cellWidth;
    int d_cellHeight;

    // remaining player moves
    int d_currentMoveCount;

    // current objectives state
    std::map<Gem::Color, sf::Text> d_currentObjectives;

    float d_gemPositioningTweakFactor;
    float d_targetGemSize;

    // vector that holds all gems
    std::vector<std::shared_ptr<Gem>> d_gemArray;
    std::vector<std::shared_ptr<Gem>> d_pendingRemovalGemArray;

    // holds the current board state
    DrawBoardState d_state;

    // track selection
    int d_selectedCell;

    // holds info if gem is selected
    bool d_isGemSelected;

    // is animation playing
    bool d_isAnimating;

    // holds objective gems texture
    std::unique_ptr<sf::RenderTexture> d_combinedObjectivesGemTexture;

    // holds score board background texture
    std::unique_ptr<sf::RenderTexture> d_combinedScoreTexture;

    // holds game board background texture
    std::unique_ptr<sf::RenderTexture> d_combinedTileTexture;

    // views actual score
    std::unique_ptr<sf::Text> d_score;

    sf::Font d_scoreFont;

    // gameplay handler functions
    std::function<void(int, int, int, int)> d_GemsSelected;
    std::function<bool(int, int)> d_BombSelected;

    // animation handler functions
    std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> d_PlayScaleAnimation;
    std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> d_PlayMoveHorizAnimation;
    std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> d_PlayMoveVertAnimation;

    // resolution of view board will be drawn on
    int d_ResolutionW;
    int d_ResolutionH;
};
