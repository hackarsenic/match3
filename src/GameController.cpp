#include <fstream>
#include <SFML/Graphics.hpp>

#include "GameController.hpp"
#include <functional>
#include "DrawBoard.hpp"
#include "BoardController.hpp"
#include "Animation.h"
#include "resources.h"
#include "json.hpp"

using json = nlohmann::json;

bool GameController::Initialize()
{
    // TODO check if file exists, if not init with default values
    std::ifstream config_file(std::string(PATH_TO_RECOURCES) + "config.json");
    json config = json::parse(config_file);

    m_ColSize = config["col_size"];
    m_RowSize = config["row_size"];

    InitWindowSize();

	_app = new RenderWindow(VideoMode(m_WindowWidth, m_WindowHeight), "Game", Style::Close);
	_app->setFramerateLimit(60);

	// create the board controller
	m_BoardController = new BoardController(m_ColSize, m_RowSize, m_CellSize);

	return true;
}

void GameController::InitWindowSize()
{
    if (m_RowSize >= m_ColSize)
    {
        m_WindowWidth = (m_CellSize - (5 * m_RowSize)) * m_ColSize;
        m_WindowHeight = (m_CellSize - (5 * m_RowSize)) * (m_RowSize + 2); //plus two reserve for scoreboard
    }
    else
    {
        m_WindowWidth = (m_CellSize - (5 * m_ColSize)) * m_ColSize;
        m_WindowHeight = (m_CellSize - (5 * m_ColSize)) * (m_RowSize + 2); //plus two reserve for scoreboard
    }

    m_WindowWidth += m_CellSize * 2;
    m_WindowHeight += m_CellSize * 2;
}

void GameController::CleanUp()
{
	delete m_BoardController;
}


int GameController::Execute()
{
	// try to initialize the app
	if (!Initialize())
	{
		return 0;
	}

    Animation animation;
	while (_app->isOpen()) {
		_app->clear(Color(50, 50, 50, 255));
		_app->draw(*m_BoardController->GetDrawBoard());
		animation.SetDrawBoard(m_BoardController->GetDrawBoard());
		animation.update(0.3f);

		m_BoardController->Update();
		sf::Event event;
		while (_app->pollEvent(event)) {
			m_BoardController->GetDrawBoard()->OnMousePress(&event);

			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				_app->close();
		}
		_app->display();
	}

	CleanUp();

	return 1;
}

