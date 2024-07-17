
#pragma once

#include <SFML/Graphics.hpp>
#include <stdio.h>

#include "BoardController.h"
#include "DrawBoard.h"

using namespace sf;

class GameController
{
public:
    enum class GameStatus
    {
        Started,
        Won,
        Failed
    };
    GameController();

    // Execute the app
    // return 0 if exits normal
    int Execute();

private:
    bool Initialize();
    void InitWindowSize();

    // inital window dimensions
    uint16_t g_windowWidth;
    uint16_t g_windowHeight;

    // controller that handles game logic
    std::unique_ptr<BoardController> g_boardController;
    std::unique_ptr<RenderWindow> g_appRenderWindow;
    GameStatus g_gameStatus;
};