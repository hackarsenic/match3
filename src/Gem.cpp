#include "Gem.hpp"
#include "SFML/Graphics.hpp"

Gem::Gem(Color color) :
	_Color(color),
	_State(State::Normal)
{
	// create sprites for all gem colors
	sf::Sprite *red_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/red.png"));
	sf::Sprite *green_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/green.png"));
	sf::Sprite *blue_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/blue.png"));
	sf::Sprite *violet_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/violet.png"));
	sf::Sprite *orange_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/orange.png"));
	sf::Sprite *bomb_v_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/v_bomb.png"));
	sf::Sprite *bomb_h_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/h_bomb.png"));
	sf::Sprite *bomb_sprite = new sf::Sprite(ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/bomb.png"));

	// add sprites to the map
	_ColorSpriteMap[Gem::Color::Red] = red_sprite;
	_ColorSpriteMap[Gem::Color::Green] = green_sprite;
	_ColorSpriteMap[Gem::Color::Blue] = blue_sprite;
	_ColorSpriteMap[Gem::Color::Violet] = violet_sprite;
	_ColorSpriteMap[Gem::Color::Orange] = orange_sprite;
	_ColorSpriteMap[Gem::Color::Bomb_V] = bomb_v_sprite;
	_ColorSpriteMap[Gem::Color::Bomb_H] = bomb_h_sprite;
	_ColorSpriteMap[Gem::Color::Bomb] = bomb_sprite;
}

Gem::~Gem()
{
	delete _ColorSpriteMap[Gem::Color::Red];
	delete _ColorSpriteMap[Gem::Color::Green];
	delete _ColorSpriteMap[Gem::Color::Blue];
	delete _ColorSpriteMap[Gem::Color::Violet];
	delete _ColorSpriteMap[Gem::Color::Orange];
	delete _ColorSpriteMap[Gem::Color::Bomb_V];
	delete _ColorSpriteMap[Gem::Color::Bomb_H];
	delete _ColorSpriteMap[Gem::Color::Bomb];

	_ColorSpriteMap.clear();
}

void Gem::ApplySprite()
{
	_Sprite = _ColorSpriteMap[_Color];
}

void Gem::SetState(State newState)
{
	_State = newState;
}

Gem::State const& Gem::GetState()
{
	return _State;
}
