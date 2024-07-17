#include "GameConfig.h"
#include "DrawSplashScreen.h"

DrawSplashScreen::DrawSplashScreen(const sf::Color &color, const std::string &text) :
    d_Sprite(std::make_unique<sf::Sprite>())
{
    const uint16_t width = GameConfig::GetInstance().GetCellSize() * 5;
    const uint16_t height = GameConfig::GetInstance().GetCellSize() * 4;
    sf::Image img;
    img.create(width, height, color);
    sf::Texture win_texture;
    win_texture.create(width, height);

    win_texture.update(img);
    d_Sprite->setTexture(win_texture);

    d_TextFont.loadFromFile(std::string(PATH_TO_RECOURCES) + "arial.ttf");
    sf::Text win_text(text, d_TextFont);
    win_text.setCharacterSize(70);
    const auto text_l_bbox = win_text.getLocalBounds();
    win_text.setOrigin(text_l_bbox.left + text_l_bbox.width / 2.0f,
                       text_l_bbox.top + text_l_bbox.height / 2.0f);

    // Set the text position to the center of the sprite
    const auto sprite_g_bbox = d_Sprite->getGlobalBounds();
    win_text.setPosition(sprite_g_bbox.left + sprite_g_bbox.width / 2.0f,
                         sprite_g_bbox.top + sprite_g_bbox.height / 2.0f);

    d_RTexture.create(width, height);
    d_RTexture.draw(*d_Sprite);
    d_RTexture.draw(win_text);
    d_RTexture.display();
    d_Sprite->setTexture(d_RTexture.getTexture());
}

bool DrawSplashScreen::OnMousePress(sf::Event *event) const
{
    if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left) {
        const auto mouse_pos_x = event->mouseButton.x;
        const auto mouse_pos_y = event->mouseButton.y;

        if (mouse_pos_x < 0 || mouse_pos_y < 0)
            return false;

        const auto sprite_g_bbox = d_Sprite->getGlobalBounds();
        return mouse_pos_x >= sprite_g_bbox.left && mouse_pos_x < sprite_g_bbox.left + sprite_g_bbox.width &&
               mouse_pos_y >= sprite_g_bbox.top && mouse_pos_y < sprite_g_bbox.top + sprite_g_bbox.height;
    }
    return false;
}

void DrawSplashScreen::draw(sf::RenderTarget &target, sf::RenderStates ) const
{
    const auto sprite_l_bbox = d_Sprite->getLocalBounds();
    d_Sprite->setOrigin(sprite_l_bbox.left + sprite_l_bbox.width / 2.0f,
                           sprite_l_bbox.top + sprite_l_bbox.height / 2.0f);
    d_Sprite->setPosition(target.getSize().x / 2.0f, target.getSize().y / 2.0f);
    target.draw(*d_Sprite);
}
