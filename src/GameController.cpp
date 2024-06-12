#include <fstream>
#include <SFML/Graphics.hpp>

#include "GameController.h"
#include "BoardController.h"
#include "Animation.h"
#include "resources.h"
#include "json.hpp"

using json = nlohmann::json;

GameController::GameController() : g_cellSize(110), g_windowWidth(0), g_windowHeight(0), g_rowSize(5), g_colSize(5), g_gameStatus(GameStatus::NotStarted) { }

bool GameController::Initialize()
{
    std::ifstream config_file(std::string(PATH_TO_RECOURCES) + "config.json", std::ifstream::in);
    if (config_file.good()) {
        json config = json::parse(config_file);

        g_colSize = config["col_size"];
        g_rowSize = config["row_size"];
    }

    config_file.close();
    InitWindowSize();

    g_appRenderWindow = std::make_unique<RenderWindow>(VideoMode(g_windowWidth, g_windowHeight), "Game", Style::Close);
    g_appRenderWindow->setFramerateLimit(60);

    // create the board controller
    g_boardController = std::make_unique<BoardController>(g_colSize, g_rowSize, g_cellSize);

    return true;
}

void GameController::InitWindowSize()
{
    if (g_rowSize >= g_colSize) {
        g_windowWidth = (g_cellSize - (5 * g_rowSize)) * g_colSize;
        g_windowHeight = (g_cellSize - (5 * g_rowSize)) * (g_rowSize + 2); // plus two reserve for scoreboard
    } else {
        g_windowWidth = (g_cellSize - (5 * g_colSize)) * g_colSize;
        g_windowHeight = (g_cellSize - (5 * g_colSize)) * (g_rowSize + 2); // plus two reserve for scoreboard
    }

    g_windowWidth += g_cellSize * 2;
    g_windowHeight += g_cellSize * 2;
}

int GameController::Execute()
{
    // try to initialize the app
    if (!Initialize()) {
        return 0;
    }

    Animation animation;
    while (g_appRenderWindow->isOpen()) {
        g_gameStatus = GameStatus::Started;
        g_appRenderWindow->clear(Color(50, 50, 50, 255));
        g_appRenderWindow->draw(*g_boardController->GetDrawBoard());
        animation.SetDrawBoard(g_boardController->GetDrawBoard());
        animation.update(0.3f);

        g_boardController->Update();
        sf::Event event;
        while (g_appRenderWindow->pollEvent(event)) {
            g_boardController->GetDrawBoard()->OnMousePress(&event);

            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                g_appRenderWindow->close();
        }
        g_appRenderWindow->display();
    }

    return 1;
}
