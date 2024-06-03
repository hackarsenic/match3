#pragma once

#include <memory>
#include <cstdint>
#include <string>
#include "ResourceManager.h"
#include "SFML/Graphics.hpp"
#include <unordered_map>

/*! Holds the visual representation of a color
*
*/
class Gem 
{
public:
	// gem colors
	enum class Color { Red, Green, Blue, Violet, Orange, COUNT = 5, Bomb_V, Bomb_H, Bomb};
	// gem states
	enum class State { Normal, Selected, Animated, Removed };

    explicit Gem(Color color);
	~Gem();

	// applies gem sprite according to color
	void ApplySprite();

	// change gem state
	void SetState(const State &newState);

	// get gem state
	const State &GetState() const;

	// get gem sprite
    const std::unique_ptr<sf::Sprite>& GetSprite() const { return g_sprite; }

private:
	Color g_color;
	State g_state;
	float g_scaleX;
	float g_scaleY;

//	sf::Sprite* _Sprite;
	std::unique_ptr<sf::Sprite> g_sprite;

	// map containing color and sprite
	std::unordered_map <Gem::Color, std::unique_ptr<sf::Sprite>> g_colorSpriteMap;
};
