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

    void SetDrawBoard(const std::shared_ptr<DrawBoard> &draw_board);
    void onAnimationScalePlay(const std::unique_ptr<Gem> &gem, const sf::Vector2f &scale_to);
    void onAnimationMoveHorizPlay(const std::unique_ptr<Gem> &gem, const sf::Vector2f &move_to);
    void onAnimationMoveVertPlay(const std::unique_ptr<Gem> &gem, const sf::Vector2f &move_to);
    void update(float delta_time);

private:
    std::shared_ptr<DrawBoard> a_drawBoard;
    std::map<Gem *, std::tuple<AnimationType, sf::Vector2f, sf::Vector2f>> a_animateSprites;
};
