#pragma once

#include <memory>
#include <cstdint>
#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"

/*! Holds the visual representation of a color
 *
 */
class Gem
{
public:
    // gem colors
    enum class Color
    {
        Red,
        Green,
        Blue,
        Violet,
        Orange,
        COUNT = 5,
        Bomb_V,
        Bomb_H,
        Bomb
    };
    // gem states
    enum class State
    {
        Normal,
        Selected,
        Animated,
        Removed
    };

    explicit Gem(Color color);
    Gem(const Gem &rhs);
    Gem(Gem &&rhs) noexcept;

    // applies gem sprite according to color
    void ApplySprite();

    // change gem state
    void SetState(const State &newState);

    // get gem state
    [[nodiscard]] const State &GetState() const;

    // get gem sprite
    [[nodiscard]] const std::shared_ptr<sf::Sprite> &GetSprite() const;

    static const std::unordered_map<Gem::Color, sf::Sprite> &GetGemSpriteMap();

private:
    Color g_color;
    State g_state;

    std::shared_ptr<sf::Sprite> g_sprite;
};
