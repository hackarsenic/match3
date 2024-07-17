#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

class DrawSplashScreen : public sf::Drawable
{
public:
    DrawSplashScreen(const sf::Color &color, const std::string &text);
    bool OnMousePress(sf::Event *event) const;

private:
    void draw(sf::RenderTarget &target, sf::RenderStates ) const override;

private:
    sf::Font d_TextFont;
    sf::RenderTexture d_RTexture;
    std::unique_ptr<sf::Sprite> d_Sprite;
};
