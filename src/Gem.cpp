#include "Gem.h"
#include "GameConfig.h"

Gem::Gem(Color color) : g_color(color), g_state(State::Normal)
{
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
    g_sprite = std::make_shared<sf::Sprite>(GetGemSpriteMap().at(g_color));
}

void Gem::SetState(const State &newState)
{
    g_state = newState;
}

Gem::State const &Gem::GetState() const
{
    return g_state;
}

const std::shared_ptr<sf::Sprite> &Gem::GetSprite() const
{
    return g_sprite;
}

const std::unordered_map<Gem::Color, sf::Sprite> &Gem::GetGemSpriteMap()
{
    const static std::unordered_map<Gem::Color, sf::Sprite> color_sprite_map = {
        {Gem::Color::Red, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "red.png").string()))},
        {Gem::Color::Green, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "green.png").string()))},
        {Gem::Color::Blue, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "blue.png").string()))},
        {Gem::Color::Violet, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "violet.png").string()))},
        {Gem::Color::Orange, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "orange.png").string()))},
        {Gem::Color::Bomb_V, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "v_bomb.png").string()))},
        {Gem::Color::Bomb_H, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "h_bomb.png").string()))},
        {Gem::Color::Bomb, sf::Sprite(ResourceManager::GetTexture((GameConfig::GetInstance().GetResourcePath() / "bomb.png").string()))}
    };

    return color_sprite_map;
}