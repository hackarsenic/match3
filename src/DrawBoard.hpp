#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "Gem.hpp"

class Gem;

// Visible part of game

class DrawBoard : public sf::Drawable
{
public:
	DrawBoard(int columns, int rows);
	virtual ~DrawBoard();

	enum class DrawBoardState { SelectFirstGemState, SelectSecondGemState, RemoveGemsState };

	// position of the board

	void SetPosition(int x, int y);


	//set cell size according to column, row quantity and
	//actual boar width height
	void InitBoardDimentions();

	// gets gem of selectes cell
	Gem* GetGem(int column, int row) const;

	// adds gem to cell of specified color
	void Add(int column, int row, Gem::Color color);

	// removes gem from cell 
	void Remove(int column, int row);

	// moves gem to target cell
	void Move(Gem* gem, int column, int row);

	// method registers gem selection event
	void GemsSelectedHandler(std::function<void(int, int, int, int)> handlerFunction);
	void BombSelectedHandler(std::function<bool(int, int)> handlerFunction);

	// get generates board
	sf::RenderTexture &GetTileBoardTexture();

	// registers mouse events
	void OnMousePress(sf::Event* event);

private:

	//set board width
	void SetWidth(int width);

	//set board height
	void SetHeight(int height);
	
	// Generates background texture
	void GenerateTileBoardTexture();

	// Overrides draw funtion of base Drawable
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// Swaps selected cells
	void SwapCells(int first_cell, int second_cell);

	// Try blow bomb if found
	bool DetonateBomb(int cell);
	
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

	// vector that holds all gems
	std::vector<Gem*> _GemArray;

	// holds the current board state
	DrawBoardState _State;

	// track selection
	int _SelectedCell;

	// holds info if gem is selected
	bool _IsGemSelected;

	// holds board background texture
	sf::RenderTexture _CombinedTileTexture;

	// handler function
	std::function <void(int, int, int, int)> Gems_Selected;
	std::function <bool(int, int)> Bomb_Selected;

};
