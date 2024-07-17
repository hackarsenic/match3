#include "Gem.h"
#include "SFML/Graphics.hpp"
#include "resources.h"

std::unordered_map<Gem::Color, sf::Sprite> Gem::g_colorSpriteMap = {
    {Gem::Color::Red, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "red.png"))},
    {Gem::Color::Green, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "green.png"))},
    {Gem::Color::Blue, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "blue.png"))},
    {Gem::Color::Violet, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "violet.png"))},
    {Gem::Color::Orange, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "orange.png"))},
    {Gem::Color::Bomb_V, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "v_bomb.png"))},
    {Gem::Color::Bomb_H, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "h_bomb.png"))},
    {Gem::Color::Bomb, sf::Sprite(ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "bomb.png"))}
};

Gem::Gem(Color color) : g_color(color), g_state(State::Normal)
{
//    const auto min_sprite_height = std::min_element(g_colorSpriteMap.begin(), g_colorSpriteMap.end(), [](const auto &a, const auto &b) {
//        return a.second.getLocalBounds().height < b.second.getLocalBounds().height;
//    })->second.getLocalBounds().height;
//
//    for (auto& [color, sprite] : g_colorSpriteMap) {
//        const auto scale = min_sprite_height / sprite.getLocalBounds().height;
//        sprite.setScale(scale, scale);
//    }
}

Gem::Gem(const Gem &rhs)
{
    g_color = rhs.g_color;
    g_state = rhs.g_state;

    g_sprite = std::make_shared<sf::Sprite>(*rhs.g_sprite);
}

Gem::Gem(Gem &&rhs) noexcept
{
    g_color = rhs.g_color;
    g_state = rhs.g_state;

    g_sprite = std::move(rhs.g_sprite);
}

void Gem::ApplySprite()
{
    g_sprite = std::make_shared<sf::Sprite>(g_colorSpriteMap[g_color]);
}

void Gem::SetState(const State &newState)
{
    g_state = newState;
}

Gem::State const &Gem::GetState() const
{
    return g_state;
}
