#pragma once

#include <cstdint>
#include <string>
#include "ResourceManager.hpp"
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
	enum class State { Normal, Selected };

	Gem(Color color);
	virtual ~Gem();

	// applies gem sprite according to color
	void ApplySprite();

	// change gem state
	void SetState(State newState);
	// get gem state
	const State& GetState();

	// get gem sprite
	sf::Sprite* GetSprite() const { return _Sprite; }

private:

	Color _Color;
	State _State;

	sf::Sprite* _Sprite;
	
	// map containing color and sprite
	std::unordered_map <Gem::Color, sf::Sprite*> _ColorSpriteMap;
};
