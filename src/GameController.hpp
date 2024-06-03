
#pragma once

#include <SFML/Graphics.hpp>
#include <stdio.h>


#include "BoardController.hpp"
#include "DrawBoard.hpp"

using namespace sf;

enum GameStatus {
    NotStarted,
    Started,
    Won,
    Failed
};

class GameController {
public:
    GameController() = default;

    // Execute the app
    // return 0 if exits normal
	int Execute();

private:

	bool Initialize();
	void InitWindowSize();

	// free all resources
	void CleanUp();

	// inital window dimensions
	uint32_t m_WindowWidth;
	uint32_t m_WindowHeight;
    const uint32_t m_CellSize = 110;
	uint32_t m_RowSize;
	uint32_t m_ColSize;


	//input
	//InputManager* m_InputMgr = nullptr;

	// controller that handles game logic
	BoardController* m_BoardController = nullptr;

	RenderWindow *_app;
	GameStatus _gameStatus = NotStarted;
	
};