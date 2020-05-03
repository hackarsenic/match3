
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

	/*! \brief Executes the application
	*
	*	\return Returns 0 if everything is ok and 1 if there was an error
	*/
	int Execute();

private:

	/*! \brief Executes the application
	*
	*/
	bool Initialize();

	// free all resources
	void CleanUp();

	// inital window dimensions
	static const uint32_t m_WindowWidth = 755;
	static const uint32_t m_WindowHeight = 600;

	//input
	//InputManager* m_InputMgr = nullptr;

	// controller that handles game logic
	BoardController* m_BoardController = nullptr;

	RenderWindow *_app;
	GameStatus _gameStatus = NotStarted;
	
};