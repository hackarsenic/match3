#include <SFML/Graphics.hpp>

#include "GameController.h"
#include "BoardController.h"
#include "Animation.h"
#include "GameConfig.h"
#include "DrawSplashScreen.h"

GameController::GameController()
    : g_windowWidth(0),
      g_windowHeight(0),
      g_gameStatus(GameStatus::Started)
{
}

bool GameController::Initialize()
{
    InitWindowSize();

    g_appRenderWindow = std::make_unique<RenderWindow>(VideoMode(g_windowWidth, g_windowHeight), "Game", Style::Close);
    g_appRenderWindow->setFramerateLimit(60);

    // create the board controller
    g_boardController = std::make_unique<BoardController>(g_windowWidth, g_windowHeight);

    return true;
}

void GameController::InitWindowSize()
{
    const auto &config = GameConfig::GetInstance();

    g_windowWidth = config.GetCellSize() * (config.GetColSize() + 2);
    g_windowHeight = config.GetCellSize() * (config.GetRowSize() + 5);
}

int GameController::Execute()
{
    // try to initialize the app
    if (!Initialize()) {
        return 0;
    }

    Animation animation;
    DrawSplashScreen win_screen(sf::Color(30, 170, 110), "You Won!!!");
    DrawSplashScreen failed_screen(sf::Color(165, 39, 23), "Game Over");

    while (g_appRenderWindow->isOpen()) {
        sf::Event event;
        g_appRenderWindow->clear(Color(50, 50, 50, 255));

        switch (g_gameStatus) {
            case GameStatus::Started :
                g_appRenderWindow->draw(*g_boardController->GetDrawBoard());
                animation.SetDrawBoard(g_boardController->GetDrawBoard());
                animation.update(0.3f);
                g_boardController->Update();
                while (g_appRenderWindow->pollEvent(event)) {
                    g_boardController->GetDrawBoard()->OnMousePress(&event);

                    // "close requested" event: we close the window
                    if (event.type == sf::Event::Closed)
                        g_appRenderWindow->close();
                }
                if (g_boardController->GetWinStatus()) {
                    g_boardController.reset();
                    g_gameStatus = GameStatus::Won;
                } else if (g_boardController->GetGameOverStatus()) {
                    g_boardController.reset();
                    g_gameStatus = GameStatus::Failed;
                }
                break;
            case GameStatus::Won :
                g_appRenderWindow->draw(win_screen);
                while (g_appRenderWindow->pollEvent(event)) {
                    if (win_screen.OnMousePress(&event)) {
                        g_boardController = std::make_unique<BoardController>(g_windowWidth, g_windowHeight);
                        g_gameStatus = GameStatus::Started;
                    }

                    // "close requested" event: we close the window
                    if (event.type == sf::Event::Closed)
                        g_appRenderWindow->close();
                }
                break;
            case GameStatus::Failed :
                g_appRenderWindow->draw(failed_screen);
                while (g_appRenderWindow->pollEvent(event)) {
                    if (failed_screen.OnMousePress(&event)) {
                        g_boardController = std::make_unique<BoardController>(g_windowWidth, g_windowHeight);
                        g_gameStatus = GameStatus::Started;
                    }

                    // "close requested" event: we close the window
                    if (event.type == sf::Event::Closed)
                        g_appRenderWindow->close();
                }
                break;
        }

        g_appRenderWindow->display();
    }

    return 1;
}
