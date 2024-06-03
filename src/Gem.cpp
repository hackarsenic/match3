#include "Gem.hpp"
#include "SFML/Graphics.hpp"
#include "resources.h"

Gem::Gem(Color color) :
    g_color(color),
    g_scaleX(1),
    g_scaleY(1),
    g_state(State::Normal)
{
	// add sprites to the map
	g_colorSpriteMap[Gem::Color::Red]    = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "red.png")));
	g_colorSpriteMap[Gem::Color::Green]  = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "green.png")));
	g_colorSpriteMap[Gem::Color::Blue]   = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "blue.png")));
	g_colorSpriteMap[Gem::Color::Violet] = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "violet.png")));
	g_colorSpriteMap[Gem::Color::Orange] = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "orange.png")));
	g_colorSpriteMap[Gem::Color::Bomb_V] = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "v_bomb.png")));
	g_colorSpriteMap[Gem::Color::Bomb_H] = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "h_bomb.png")));
	g_colorSpriteMap[Gem::Color::Bomb]   = std::make_unique<sf::Sprite>(sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "bomb.png")));
}

Gem::~Gem()
{
	g_colorSpriteMap.clear();
}

void Gem::ApplySprite()
{
    g_sprite = std::move(g_colorSpriteMap[g_color]);
}

void Gem::SetState(const State &newState)
{
    g_state = newState;
}

Gem::State const& Gem::GetState() const
{
	return g_state;
}
