#pragma once

#include <map>
#include <memory>

#include "SFML/Graphics.hpp"
#include "DrawBoard.h"

class Animation
{
public:
    enum class AnimationType
    {
        Scale,
        MoveHoriz,
        MoveVert
    };

    // set draw board on which to play animations
    void SetDrawBoard(const std::shared_ptr<DrawBoard> &draw_board);
    // callback function for scale animation
    void onAnimationScalePlay(const std::shared_ptr<Gem> &gem, const sf::Vector2f &scale_to);
    // callback function for scale animation
    void onAnimationMoveHorizPlay(const std::shared_ptr<Gem> &gem, const sf::Vector2f &move_to);
    // callback function for scale animation
    void onAnimationMoveVertPlay(const std::shared_ptr<Gem> &gem, const sf::Vector2f &move_to);
    // play registered animations with set speed
    void update(float delta_time);

private:
    std::shared_ptr<DrawBoard> a_drawBoard;
    std::map<std::shared_ptr<Gem>, std::tuple<AnimationType, sf::Vector2f, sf::Vector2f>> a_animateSprites;
};
