#include "Animation.h"

void Animation::SetDrawBoard(const std::shared_ptr<DrawBoard> &draw_board)
{
    a_drawBoard = draw_board;
    a_drawBoard->ScaleAnimationHandler([this](const std::shared_ptr<Gem> &gem, const sf::Vector2f &scale_to) { return onAnimationScalePlay(gem, scale_to); });
    a_drawBoard->MoveHorizAnimationHandler([this](const std::shared_ptr<Gem> &gem, const sf::Vector2f &move_to) { return onAnimationMoveHorizPlay(gem, move_to); });
    a_drawBoard->MoveVertAnimationHandler([this](const std::shared_ptr<Gem> &gem, const sf::Vector2f &move_to) { return onAnimationMoveVertPlay(gem, move_to); });
}

void Animation::onAnimationScalePlay(const std::shared_ptr<Gem> &gem, const sf::Vector2f &scale_to)
{
    a_drawBoard->d_isAnimating = true;
    gem->SetState(Gem::State::Animated);
    a_animateSprites[gem] = std::make_tuple(AnimationType::Scale, sf::Vector2f(gem->GetSprite()->getScale().x, gem->GetSprite()->getScale().y), scale_to);
}

void Animation::onAnimationMoveHorizPlay(const std::shared_ptr<Gem> &gem, const sf::Vector2f &move_to)
{
    a_drawBoard->d_isAnimating = true;
    gem->SetState(Gem::State::Animated);
    a_animateSprites[gem] = std::make_tuple(AnimationType::MoveHoriz, sf::Vector2f(gem->GetSprite()->getPosition().x, gem->GetSprite()->getPosition().y), move_to);
}

void Animation::onAnimationMoveVertPlay(const std::shared_ptr<Gem> &gem, const sf::Vector2f &move_to)
{
    a_drawBoard->d_isAnimating = true;
    gem->SetState(Gem::State::Animated);
    a_animateSprites[gem] = std::make_tuple(AnimationType::MoveVert, sf::Vector2f(gem->GetSprite()->getPosition().x, gem->GetSprite()->getPosition().y), move_to);
}

void Animation::update(float duration)
{
    for (auto it = a_animateSprites.begin(); it != a_animateSprites.end();) {
        float cur_scale = it->first->GetSprite()->getScale().x;
        float cur_pos_x = it->first->GetSprite()->getPosition().x;
        float cur_pos_y = it->first->GetSprite()->getPosition().y;

        const sf::Vector2f orig = std::get<1>(it->second);
        const sf::Vector2f dest = std::get<2>(it->second);
        const sf::Vector2f step = (dest - orig) / 60.0f / duration;

        switch (std::get<0>(it->second)) {
            case AnimationType::Scale:
                cur_scale += step.x;
                it->first->GetSprite()->setScale(cur_scale, cur_scale);
                if (orig.x < dest.x) {
                    if (cur_scale >= dest.x) {
                        it->first->SetState(Gem::State::Normal);
                        it = a_animateSprites.erase(it);
                    } else
                        ++it;
                } else {
                    if (cur_scale <= dest.x) {
                        it->first->SetState(Gem::State::Normal);
                        it = a_animateSprites.erase(it);
                    } else
                        ++it;
                }
                break;
            case AnimationType::MoveHoriz:
                it->first->GetSprite()->move(step);
                if (orig.x < dest.x) {
                    if (cur_pos_x >= dest.x) {
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = a_animateSprites.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    if (cur_pos_x <= dest.x) {
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = a_animateSprites.erase(it);
                    } else {
                        ++it;
                    }
                }
                break;
            case AnimationType::MoveVert:
                it->first->GetSprite()->move(step);
                if (orig.y < dest.y) {
                    if (cur_pos_y >= dest.y) {
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = a_animateSprites.erase(it);
                    } else
                        ++it;
                } else {
                    if (cur_pos_y <= dest.y) {
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = a_animateSprites.erase(it);
                    } else
                        ++it;
                }
                break;
            default:
                return;
        }
    }

    if (a_animateSprites.empty()) {
        a_drawBoard->d_isAnimating = false;
    }
}
