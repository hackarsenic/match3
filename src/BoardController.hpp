#pragma once

#include "BoardLogic.hpp"
#include "DrawBoard.hpp"

class BoardLogic;
class DrawBoard;

// Controls DrawBoard and BoardLogic behavior
class BoardController
{
public:
	BoardController();
	~BoardController();

	enum class BoardState { Default, GemsSwapState, GemsSelectState, PatternDetectState, PatternsRemovedState, GemsSpawnState };

	// update board state
	void Update();

	// get pointer to DrawBoard
	DrawBoard* GetDrawBoard() const { return _DrawBoard; }

private:
	
	// swap gems if pattern is found
	void TrySwapGems();

	// detect color pattern
	void FindPattern();

	// remove color pattern 
	void RemovePattern();

	// Adds new gems 
	void SpawnGems();

	// callback function for two gems have been selected
	void OnGemSelect(int source_x, int source_y, int target_x, int target_y);

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
	
	BoardState _BoardState;
	BoardLogic* _BoardLogic;
	DrawBoard* _DrawBoard;

	// store selected gems
	int _SourceCellX;
	int _SourceCellY;
	int _TargetCellX;
	int _TargetCellY;
};
