#pragma once

#include <map>
#include <memory>

#include "SFML/Graphics.hpp"
#include "DrawBoard.hpp"


class Animation {
public:
    enum class AnimationType { Scale, MoveHoriz, MoveVert };

    Animation() = default;
    void SetDrawBoard(DrawBoard *draw_board);
    void onAnimationScalePlay(Gem *gem, const sf::Vector2f &scale_to);
    void onAnimationMoveHorizPlay(Gem *gem, const sf::Vector2f &move_to);
    void onAnimationMoveVertPlay(Gem *gem, const sf::Vector2f &move_to);
    void update(float delta_time);

private:
    DrawBoard *_drawBoard;
    std::map<Gem *, std::tuple<AnimationType, sf::Vector2f, sf::Vector2f>> _animateSprites;
};
