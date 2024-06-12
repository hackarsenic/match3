#include <sstream>
#include <cmath>
#include <algorithm>
#include "DrawBoard.h"
#include "resources.h"

bool DrawBoard::d_scFlag = true;
int DrawBoard::d_ResolutionW = 0;
int DrawBoard::d_ResolutionH = 0;

// sf::RenderTexture DrawBoard::_CombinedScoreTexture;

DrawBoard::DrawBoard(int columns, int rows, int cell_size)
    : d_columns(columns),
      d_rows(rows),
      d_positionX(0),
      d_positionY(0),
      d_width(100),
      d_height(100),
      d_cellWidth(cell_size),
      d_cellHeight(cell_size),
      d_mousePositionX(0),
      d_mousePositionY(0),
      d_gemPositioningTweakFactor(13.0f),
      d_targetGemSize(0.0f),
      d_state(DrawBoardState::SelectFirstGemState),
      d_selectedCell(0),
      d_isGemSelected(false),
      d_isAnimating(false),
      d_combinedScoreTexture(std::make_unique<sf::RenderTexture>()),
      d_combinedTileTexture(std::make_unique<sf::RenderTexture>())
{
    // create array of gems
    d_gemArray.resize(d_columns * d_rows);

    // initialize board dimensions
    InitBoardDimensions();

    // create background board texture
    GenerateTileBoardTexture();
}

void DrawBoard::SetPosition(int x, int y)
{
    d_positionX = x;
    d_positionY = y;
}

constexpr void DrawBoard::SetWidth(int width)
{
    d_width = width;
}

constexpr void DrawBoard::SetHeight(int height)
{
    d_height = height;
}

constexpr void DrawBoard::InitBoardDimensions()
{
    if (d_rows >= d_columns) {
        d_cellWidth = d_cellWidth - (5 * d_rows);
        d_cellHeight = d_cellHeight - (5 * d_rows);
    } else {
        d_cellWidth = d_cellWidth - (5 * d_columns);
        d_cellHeight = d_cellHeight - (5 * d_columns);
    }

    SetWidth(d_cellWidth * d_columns);
    SetHeight(d_cellHeight * d_rows);

    d_targetGemSize = d_cellWidth * 0.7f;

    // Draw the tiles by calculating their positions
    for (int i = 0; i < d_rows * d_columns; ++i) {
        int x = i % d_columns;
        int y = i / d_columns;

        const auto &gem = d_gemArray[i];
        if (gem != nullptr) {
            const auto &currentSprite = gem->GetSprite();

            float cur_gem_scale_factor = d_targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);

            gem->GetSprite()->setPosition(d_positionX + x * d_cellWidth + d_gemPositioningTweakFactor * cur_gem_scale_factor, d_positionY + y * d_cellHeight + d_gemPositioningTweakFactor * cur_gem_scale_factor);
            gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
        }
    }
}

std::unique_ptr<Gem> DrawBoard::GetGem(int column, int row)
{
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        return std::move(d_gemArray[row * d_columns + column]);
    }

    //	return nullptr;
}

void DrawBoard::Add(int column, int row, Gem::Color color)
{
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        std::unique_ptr<Gem> gem = std::make_unique<Gem>(color);
        gem->ApplySprite();
        gem->SetState(Gem::State::Normal);

        int gem_index = row * d_columns + column;

        int x = gem_index % d_columns;
        int y = gem_index / d_columns;
        float gem_positioning_tweak_factor = 13.0f;
        float cur_gem_scale_factor = d_targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);
        gem->GetSprite()->setPosition(d_positionX + x * d_cellWidth + gem_positioning_tweak_factor * cur_gem_scale_factor, d_positionY + y * d_cellHeight + gem_positioning_tweak_factor * cur_gem_scale_factor);
        gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);

        d_gemArray[row * d_columns + column] = std::move(gem);
    }
}

void DrawBoard::MarkRemoved(int column, int row)
{
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        auto &remove_gem = d_pendingRemovalGemArray.emplace_back(std::move(d_gemArray[row * d_columns + column]));
        remove_gem->SetState(Gem::State::Removed);
        sf::Vector2f scale_to(0.01f, 0.01f);
        d_PlayScaleAnimation(remove_gem, scale_to);
        d_state = DrawBoardState::RemoveGemsState;
    }
}

void DrawBoard::Move(std::unique_ptr<Gem> move_gem, int column, int row)
{
    // cell exists, move the gem there
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        int gem_index = row * d_columns + column;
        d_gemArray[gem_index] = std::move(move_gem);
        const auto &gem = d_gemArray[gem_index];

        int x = gem_index % d_columns;
        int y = gem_index / d_columns;
        float cur_gem_scale_factor = d_targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);

        float move_x = d_positionX + x * d_cellWidth + d_gemPositioningTweakFactor * cur_gem_scale_factor;
        float move_y = d_positionY + y * d_cellHeight + d_gemPositioningTweakFactor * cur_gem_scale_factor;
        sf::Vector2f move_to(move_x, move_y);
        if (std::floor(gem->GetSprite()->getPosition().x) == std::floor(move_x))
            d_PlayMoveVertAnimation(gem, move_to);
        else
            d_PlayMoveHorizAnimation(gem, move_to);
    }
}

void DrawBoard::GemsSelectedHandler(std::function<void(int, int, int, int)> handlerFunction)
{
    d_GemsSelected = std::move(handlerFunction);
}

void DrawBoard::BombSelectedHandler(std::function<bool(int, int)> handlerFunction)
{
    d_BombSelected = std::move(handlerFunction);
}

void DrawBoard::ScaleAnimationHandler(std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> handlerFunction)
{
    d_PlayScaleAnimation = std::move(handlerFunction);
}

void DrawBoard::MoveHorizAnimationHandler(std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> handlerFunction)
{
    d_PlayMoveHorizAnimation = std::move(handlerFunction);
}

void DrawBoard::MoveVertAnimationHandler(std::function<void(const std::unique_ptr<Gem> &, const sf::Vector2f &)> handlerFunction)
{
    d_PlayMoveVertAnimation = std::move(handlerFunction);
}

void DrawBoard::OnMousePress(sf::Event *event)
{
    // check if mouse is over board
    if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left && !d_isGemSelected && !d_isAnimating) {
        // find the cell where the user clicked
        int pos_adjusted_x = event->mouseButton.x - d_positionX;
        int pos_adjusted_y = event->mouseButton.y - d_positionY;

        if (pos_adjusted_x < 0 || pos_adjusted_y < 0)
            return;

        int x_cell = (event->mouseButton.x - d_positionX) / d_cellWidth;
        int y_cell = (event->mouseButton.y - d_positionY) / d_cellHeight;

        if (x_cell > d_columns - 1)
            return;
        if (y_cell > d_rows - 1)
            return;

        if (d_state == DrawBoardState::SelectFirstGemState || d_state == DrawBoardState::RemoveGemsState) {
            d_selectedCell = y_cell * d_columns + x_cell;

            const auto &gem = d_gemArray[d_selectedCell];
            gem->SetState(Gem::State::Selected);

            if (DetonateBomb(d_selectedCell))
                return;

            d_state = DrawBoardState::SelectSecondGemState;

            const auto &current_sprite = gem->GetSprite();
            sf::Vector2f scale_to(current_sprite->getScale().x * 0.75, current_sprite->getScale().x * 0.75);
            d_PlayScaleAnimation(gem, scale_to);
        }

        d_isGemSelected = true;
        return;
    }

    if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left && d_isGemSelected) {
        // find the second cell where user clicked
        int x_cell = (event->mouseButton.x - d_positionX) / d_cellWidth;
        int y_cell = (event->mouseButton.y - d_positionY) / d_cellHeight;

        if (x_cell > d_columns - 1)
            x_cell = d_columns - 1;
        if (y_cell > d_rows - 1)
            y_cell = d_rows - 1;

        // second cell where user clicked
        int second_cell = y_cell * d_columns + x_cell;

        // user selected the second gem
        if (d_state == DrawBoardState::SelectSecondGemState) {
            // invoke gems selected event
            SwapCells(d_selectedCell, second_cell);

            const auto &gem = d_gemArray[d_selectedCell];
            gem->SetState(Gem::State::Normal);
            d_state = DrawBoardState::SelectFirstGemState;

            float cur_gem_scale_factor = d_targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);
            d_PlayScaleAnimation(gem, sf::Vector2f(cur_gem_scale_factor, cur_gem_scale_factor));
        }

        // TODO possibly unnecessary
        // user clicked same cell twice
        if (d_state == DrawBoardState::SelectSecondGemState && d_selectedCell == second_cell) {
            const auto &gem = d_gemArray[second_cell];
            gem->SetState(Gem::State::Selected);

            float target_gem_size = d_cellWidth * 0.7f;
            float cur_gem_scale_factor = target_gem_size / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);
            gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
        }

        d_isGemSelected = false;
        return;
    }
}

constexpr void DrawBoard::SwapCells(int first_cell, int second_cell)
{
    // swap if cells are adjacent
    if (((first_cell / d_columns) == (second_cell / d_columns) && abs(first_cell - second_cell) == 1) || abs(first_cell - second_cell) == d_columns) {
        // invoke gems selected event
        d_GemsSelected(d_selectedCell % d_columns, d_selectedCell / d_columns, second_cell % d_columns, second_cell / d_columns);
    }
}

bool DrawBoard::DetonateBomb(int cell)
{
    return d_BombSelected(d_selectedCell % d_columns, d_selectedCell / d_columns);
}

// TODO finish score board
void DrawBoard::GenerateScoreBoardTexture() const
{
    sf::Texture &center_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_2.png");
    sf::Sprite center_sprite(center_texture);
    int cent_texture_w = center_texture.getSize().x;
    int cent_texture_h = center_texture.getSize().y;
    float cent_texture_scale_factor_x = static_cast<float>(d_cellWidth) / cent_texture_w;
    float cent_texture_scale_factor_y = static_cast<float>(d_cellHeight) / cent_texture_h;
    center_sprite.setScale(cent_texture_scale_factor_x, cent_texture_scale_factor_y);

    sf::Texture &borders_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_1.png");
    sf::Sprite borders_sprite(borders_texture);
    int bord_texture_w = borders_texture.getSize().x;
    int bord_texture_h = borders_texture.getSize().y;
    float bord_texture_scale_factor_x = static_cast<float>(d_cellWidth) / bord_texture_w;
    float bord_texture_scale_factor_y = static_cast<float>(d_cellHeight) / bord_texture_h;
    borders_sprite.setScale(bord_texture_scale_factor_x, bord_texture_scale_factor_y);

    int render_window_width = d_ResolutionW;
    int num_of_tiles = ceil(static_cast<float>(render_window_width) / (d_cellWidth));
    if (num_of_tiles % 2 != 0)
        ++num_of_tiles;

    d_combinedScoreTexture->create(num_of_tiles * cent_texture_w * cent_texture_scale_factor_x, 2 * cent_texture_h * cent_texture_scale_factor_y);

    // need only two rows
    for (int i = 0; i < num_of_tiles; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (i == num_of_tiles / 2 || i == (num_of_tiles / 2 - 1)) {
                center_sprite.setPosition(i * cent_texture_w * cent_texture_scale_factor_x, j * cent_texture_h * cent_texture_scale_factor_y);
                d_combinedScoreTexture->draw(center_sprite);
            } else {
                borders_sprite.setPosition(i * bord_texture_w * bord_texture_scale_factor_x, j * bord_texture_h * bord_texture_scale_factor_y);
                d_combinedScoreTexture->draw(borders_sprite);
            }
        }
    }

    d_combinedScoreTexture->display();
}

void DrawBoard::GenerateTileBoardTexture() const
{
    sf::Texture &tile1_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_1.png");
    sf::Sprite tile1_sprite(tile1_texture);
    int tile1_text_size_w = tile1_sprite.getTexture()->getSize().x;
    int tile1_text_size_h = tile1_sprite.getTexture()->getSize().y;

    // scale tile1 size to cell size
    float scale_factor_x = static_cast<float>(d_cellWidth) / tile1_text_size_w;
    float scale_factor_y = static_cast<float>(d_cellHeight) / tile1_text_size_h;
    tile1_sprite.setScale(scale_factor_x, scale_factor_y);

    sf::Texture &tile2_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_2.png");
    ;
    sf::Sprite tile2_sprite(tile2_texture);
    int tile2_text_size_w = tile2_sprite.getTexture()->getSize().x;
    int tile2_text_size_h = tile2_sprite.getTexture()->getSize().y;

    // scale tile2 size to cell size
    scale_factor_x = static_cast<float>(d_cellWidth) / tile2_text_size_w;
    scale_factor_y = static_cast<float>(d_cellHeight) / tile2_text_size_h;
    tile2_sprite.setScale(scale_factor_x, scale_factor_y);

    bool seq = true;
    d_combinedTileTexture->create(d_columns * tile1_text_size_w * scale_factor_x, d_rows * tile1_text_size_h * scale_factor_y);

    // populate the vector, with two tile sprites
    for (int i = 0; i < d_columns; ++i) {
        for (int j = 0; j < d_rows; ++j) {
            if (seq) {
                tile1_sprite.setPosition(i * tile1_text_size_w * scale_factor_x, j * tile1_text_size_h * scale_factor_y);
                d_combinedTileTexture->draw(tile1_sprite);
            } else {
                tile2_sprite.setPosition(i * tile2_text_size_w * scale_factor_x, j * tile2_text_size_h * scale_factor_y);
                d_combinedTileTexture->draw(tile2_sprite);
            }
            seq = !seq;
        }
        seq = d_rows % 2 != 0 == seq;
    }

    d_combinedTileTexture->display();
}

void DrawBoard::ClearPendingRemovalGems()
{
    d_pendingRemovalGemArray.clear();
}

// const std::unique_ptr<sf::RenderTexture> &DrawBoard::GetTileBoardTexture()
//{
//     return d_combinedTileTexture;
// }

void DrawBoard::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (d_scFlag) {
        d_ResolutionW = target.getSize().y;
        d_ResolutionH = target.getSize().x;
        GenerateScoreBoardTexture();
        d_scFlag = false;
    }

    sf::Sprite score_board(d_combinedScoreTexture->getTexture());

    float score_board_scale_factor = static_cast<float>(target.getSize().x) / d_combinedScoreTexture->getSize().x;
    score_board.setScale(score_board_scale_factor, score_board_scale_factor);
    score_board.setPosition(0, 0);

    target.draw(score_board);

    sf::Sprite board(d_combinedTileTexture->getTexture());
    board.setPosition(d_positionX, d_positionY);

    board.setScale(1, 1);
    target.draw(board);

    // Draw the tiles by calculating their positions
    for (int i = 0; i < d_rows * d_columns; ++i) {
        const auto &gem = d_gemArray[i];
        if (gem != nullptr) {
            target.draw(*gem->GetSprite());
        }
    }

    for (const auto &g : d_pendingRemovalGemArray) {
        target.draw(*g->GetSprite());
    }
}
