#include <SFML/Graphics.hpp>

#include "GameController.hpp"
#include <functional>
#include "DrawBoard.hpp"
#include "BoardController.hpp"



bool GameController::Initialize()
{
	_app = new RenderWindow(VideoMode(744, 1080), "Game", Style::Close);
	_app->setFramerateLimit(60);

	// create the board controller
	m_BoardController = new BoardController();

	return true;
}

void GameController::CleanUp()
{
	delete m_BoardController;
}

int GameController::Execute()
{
	// try to initialize the app
	if (Initialize() != true)
	{
		return 0;
	}

	while (_app->isOpen()) {
		_app->clear(Color(50, 50, 50, 255));
		_app->draw(*m_BoardController->GetDrawBoard());

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

