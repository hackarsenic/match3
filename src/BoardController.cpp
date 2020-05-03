#include <iostream>
#include "BoardLogic.hpp"
#include "DrawBoard.hpp"

#include "BoardController.hpp"
#include "Gem.hpp"

BoardController::BoardController() :
	_BoardState(BoardState::Default),
	_BoardLogic(new BoardLogic(7,10)),
	_DrawBoard(new DrawBoard(7,10))
{
	// set view properties
	_DrawBoard->SetPosition(0, 0);
	
	// register callbacks
	_DrawBoard->GemsSelectedHandler([this](int sourceX, int sourceY, int targetX, int targetY) 
									{ return OnGemSelect(sourceX, sourceY, targetX, targetY); });
	_DrawBoard->BombSelectedHandler([this](int column, int row)	{ return OnBombSelect(column, row); });
	_BoardLogic->ColorAddedHandler([this](int column, int row){ return OnColorAdd(column, row); });
	_BoardLogic->ColorRemovedHandler([this](int column, int row) { return OnColorRemove(column, row); });
	_BoardLogic->ColorSpawnedHandler([this](int column, int row) { return OnColorSpawn(column, row); });
	_BoardLogic->BombAddedHandler([this](int column, int row) { return OnBombSpawn(column, row); });
	
	// generate the board
	_BoardLogic->Generate();

	// board dimensions
	int columns = _BoardLogic->GetColumns();
	int rows = _BoardLogic->GetRows();

	// get gems to draw on board
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < columns; ++x)
		{
			int color = _BoardLogic->GetGemColor(x, y);
			_DrawBoard->Add(x, y, Gem::Color(color));
		}
	}
}


BoardController::~BoardController()
{
	delete _DrawBoard;
	delete _BoardLogic;
}

void BoardController::Update()
{
	// after every update check if gems were swapped.
	// If gems were swapped check if patterns were removed
	
	switch (_BoardState)
	{
	case BoardState::GemsSelectState:
		// try to swap selected gems
		TrySwapGems();
		break;
	case BoardState::GemsSwapState:
		// find pattern
		FindPattern();
		break;
	case BoardState::PatternDetectState:
		// remove all patterns
		RemovePattern();
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

void BoardController::OnGemSelect(int source_x, int source_y, int target_x, int target_y)
{
	// store coordinates of source and target cells
	_SourceCellX = source_x;
	_SourceCellY = source_y;
	_TargetCellX = target_x;
	_TargetCellY = target_y;

	// gems selected
	_BoardState = BoardState::GemsSelectState;
}

bool BoardController::OnBombSelect(int column, int row)
{
	//Try to detonate bomb if bomb is selected
	if (_BoardLogic->DetonateBomb(column, row))
	{
		_BoardState = BoardState::PatternDetectState;
		return true;
	}
}

void BoardController::TrySwapGems()
{
	// check if the attempted swap leads to a pattern
	bool is_swap_valid = _BoardLogic->SwapGemColors(_SourceCellX, _SourceCellY, _TargetCellX, _TargetCellY);

	// get gems pending to swap
	Gem* first_gem = _DrawBoard->GetGem(_SourceCellX, _SourceCellY);
	Gem* second_gem = _DrawBoard->GetGem(_TargetCellX, _TargetCellY);

	if (is_swap_valid)
	{
		// swap gems in draw board
		_DrawBoard->Move(first_gem, _TargetCellX, _TargetCellY);
		_DrawBoard->Move(second_gem, _SourceCellX, _SourceCellY);

		// swapped successfully
		_BoardState = BoardState::GemsSwapState;
	}
}

void BoardController::FindPattern()
{
	// find matching color pattern
	_BoardLogic->DetectGemColorPatterns();
	_BoardLogic->DetectGemSquareColorPatterns();

	if (_BoardLogic->HasPatterns() == true)
		_BoardState = BoardState::PatternDetectState;
}

void BoardController::RemovePattern()
{
	_BoardLogic->RemovePattern();
	_BoardState = BoardState::PatternsRemovedState;
}

void BoardController::SpawnGems()
{
	// spawn new colors until no empty cells left
	if (_BoardLogic->GetEmptyCells() > 0)
	{
		_BoardLogic->SpawnBomb(0,0);
		_BoardLogic->SpawnColors();
	}
	else
	{
		_BoardState = BoardState::GemsSpawnState;
	}	
}

void BoardController::OnColorAdd(int column, int row)
{
	// retrieve color
	int color = _BoardLogic->GetGemColor(column, row);

	// draw new gem on board
	_DrawBoard->Add(column, row, Gem::Color(color));
}

void BoardController::OnColorRemove(int column, int row)
{
	_DrawBoard->Remove(column, row);
}

void BoardController::OnColorSpawn(int column, int row)
{
	// retrieve gem at position
	Gem* gem = _DrawBoard->GetGem(column, row);
	// move down one row
	_DrawBoard->Move(gem, column, row + 1);
}

void BoardController::OnBombSpawn(int column, int row)
{
	// retrieve color
	int color = _BoardLogic->GetGemColor(column, row);

	// draw new gem on board
	_DrawBoard->Add(column, row, Gem::Color(color));
}