#include <cmath>
#include "Animation.h"

void Animation::SetDrawBoard(DrawBoard *draw_board)
{
    _drawBoard = draw_board;
    _drawBoard->ScaleAnimationHandler([this](Gem *gem, const sf::Vector2f &scale_to) {
        return onAnimationScalePlay(gem, scale_to);
    });
    _drawBoard->MoveHorizAnimationHandler([this](Gem *gem, const sf::Vector2f &move_to) {
        return onAnimationMoveHorizPlay(gem, move_to);
    });
    _drawBoard->MoveVertAnimationHandler([this](Gem *gem, const sf::Vector2f &move_to) {
        return onAnimationMoveVertPlay(gem, move_to);
    });
}

#include <iostream>
void Animation::onAnimationScalePlay(Gem *gem, const sf::Vector2f &scale_to)
{
    _drawBoard->_isAnimating = true;
    gem->SetState(Gem::State::Animated);
    _animateSprites[gem] = std::make_tuple(AnimationType::Scale, sf::Vector2f(gem->GetSprite()->getScale().x, gem->GetSprite()->getScale().y) , scale_to);
}

void Animation::onAnimationMoveHorizPlay(Gem *gem, const sf::Vector2f &move_to)
{
    _drawBoard->_isAnimating = true;
    std::cout << "RECORD: MoveHoriz, orig_pos: " << gem->GetSprite()->getPosition().x << ", move_to: " << move_to.x << std::endl;
    gem->SetState(Gem::State::Animated);
    _animateSprites[gem] = std::make_tuple(AnimationType::MoveHoriz, sf::Vector2f(gem->GetSprite()->getPosition().x, gem->GetSprite()->getPosition().y), move_to);
}

void Animation::onAnimationMoveVertPlay(Gem *gem, const sf::Vector2f &move_to)
{
    _drawBoard->_isAnimating = true;
    std::cout << "RECORD: MoveHoriz, orig_pos: " << gem->GetSprite()->getPosition().y << ", move_to: " << move_to.y << std::endl;
    gem->SetState(Gem::State::Animated);
    _animateSprites[gem] = std::make_tuple(AnimationType::MoveVert, sf::Vector2f(gem->GetSprite()->getPosition().x, gem->GetSprite()->getPosition().y), move_to);
}

void Animation::update(float duration)
{
    for (auto it = _animateSprites.begin(); it != _animateSprites.end();) {
        float cur_scale = std::get<0>(*it)->GetSprite()->getScale().x;
        float cur_pos_x = std::get<0>(*it)->GetSprite()->getPosition().x;
        float cur_pos_y = std::get<0>(*it)->GetSprite()->getPosition().y;

        const sf::Vector2f orig = std::get<1>(it->second);
        const sf::Vector2f dest = std::get<2>(it->second);
        const sf::Vector2f step = (dest - orig) / 60.0f / duration;

        switch (std::get<0>(it->second)) {
            case AnimationType::Scale :
                cur_scale += step.x;
                it->first->GetSprite()->setScale(cur_scale, cur_scale);
                if (orig.x < dest.x) {
                    if (cur_scale >= dest.x) {
                        it->first->SetState(Gem::State::Normal); // DrawBoardState::SelectFirstGemState
                        it = _animateSprites.erase(it);
                    }
                    else
                        ++it;
                } else {
                    if (cur_scale <= dest.x) {
                        it->first->SetState(Gem::State::Normal); // DrawBoardState::SelectSecondGemState
                        it = _animateSprites.erase(it);
                    }
                    else
                        ++it;
                }
                break;
            case AnimationType::MoveHoriz :
                std::cout << "PLAY: MoveHoriz, orig: X = " << orig.x << ", Y = " << orig.y << ", move_to:  X = " << dest.x << ", Y = " <<dest.y << ", cur: " << it->first->GetSprite()->getPosition().x << std::endl;
                it->first->GetSprite()->move(step);
                if (orig.x < dest.x) {
                    if (cur_pos_x >= dest.x){
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = _animateSprites.erase(it);
                    }
                    else {
                        ++it;
                    }
                } else {
                    if (cur_pos_x <= dest.x) {
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = _animateSprites.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
                break;
            case AnimationType::MoveVert :
                std::cout << "PLAY: MoveHoriz, orig: X = " << orig.x << ", Y = " << orig.y << ", move_to:  X = " << dest.x << ", Y = " <<dest.y << ", cur: " << it->first->GetSprite()->getPosition().y << std::endl;
                it->first->GetSprite()->move(step);
                if (orig.y < dest.y) {
                    if (cur_pos_y >= dest.y) {
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = _animateSprites.erase(it);
                    }
                    else
                        ++it;
                } else {
                    if (cur_pos_y <= dest.y) {
                        it->first->GetSprite()->setPosition(dest);
                        it->first->SetState(Gem::State::Normal);
                        it = _animateSprites.erase(it);
                    }
                    else
                        ++it;
                }
                break;
            default:
                return;
        }
    }

    if (_animateSprites.empty()) {
        _drawBoard->_isAnimating = false;
    }
}
