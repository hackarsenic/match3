#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "Gem.hpp"

class Gem;

// Visible part of game

class DrawBoard : public sf::Drawable
{
public:
    friend class Animation;

	DrawBoard(int columns, int rows, int cell_size = 110);
	virtual ~DrawBoard();

	enum class DrawBoardState { SelectFirstGemState, SelectSecondGemState, RemoveGemsState };

	// position of the board
	void SetPosition(int x, int y);

	//set cell size according to column, row quantity and
	//actual board width height
	void InitBoardDimensions();

	// gets gem of selected cell
	Gem* GetGem(int column, int row) const;

	bool GetAnimationState() const { return _isAnimating; }

	// adds gem to cell of specified color
	void Add(int column, int row, Gem::Color color);

	// removes gem from cell 
	void Remove(int column, int row);

	// moves gem to target cell
	void Move(Gem *gem, int column, int row);

	// method registers gem selection event
	void GemsSelectedHandler(std::function<void(int, int, int, int)> handlerFunction);
	void BombSelectedHandler(std::function<bool(int, int)> handlerFunction);
	void ScaleAnimationHandler(std::function <void(Gem *, const sf::Vector2f &)> handlerFunction);
	void MoveHorizAnimationHandler(std::function <void(Gem *, const sf::Vector2f &)> handlerFunction);
	void MoveVertAnimationHandler(std::function <void(Gem *, const sf::Vector2f &)> handlerFunction);

	// get generates board
	sf::RenderTexture &GetTileBoardTexture();

	// registers mouse events
	void OnMousePress(sf::Event *event);

private:

	//set board width
	void SetWidth(int width);

	//set board height
	void SetHeight(int height);

	// Generates score board texture
	void GenerateScoreBoardTexture() const;

	// Generates game board texture
	void GenerateTileBoardTexture();

	// Overrides draw funtion of base Drawable
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	// Swaps selected cells
	void SwapCells(int first_cell, int second_cell);

	// Try blow bomb if found
	bool DetonateBomb(int cell);

//	void Animate(sf::Sprite *sprite, float &scale_factor) const;
	
	// amount of cells
	int _Columns;
	int _Rows;

	// position and dimensions of the board
	int _PositionX;
	int _PositionY;
	int _Width;
	int _Height;

	// dimensions of the cells
	int _CellWidth;
	int _CellHeight;

	// mouse position
	int _MousePositionX;
	int _MousePositionY;

    float _gemPositioningTweakFactor;
    float _targetGemSize;

	// resolution of view board will be drawn on
	static int _ResolutionW;
	static int _ResolutionH;

	// vector that holds all gems
	std::vector<Gem *> _GemArray;
	std::vector<Gem *> _pendingRemovalGemArray;

	// holds the current board state
	DrawBoardState _State;

	// track selection
	int _SelectedCell;

	// holds info if gem is selected
	bool _IsGemSelected;

	// holds score board background texture
    std::unique_ptr<sf::RenderTexture> _CombinedScoreTexture;

	// holds game board background texture
	sf::RenderTexture _CombinedTileTexture;

	// gameplay handler functions
	std::function <void(int, int, int, int)> Gems_Selected;
	std::function <bool(int, int)> Bomb_Selected;

	// animation handler functions
	std::function <void(Gem *, const sf::Vector2f &)> PlayScaleAnimation;
	std::function <void(Gem *, const sf::Vector2f &)> PlayMoveHorizAnimation;
	std::function <void(Gem *, const sf::Vector2f &)> PlayMoveVertAnimation;

	// flag to generate score board only once
	static bool _scFlag;

	bool _isAnimating;
};
