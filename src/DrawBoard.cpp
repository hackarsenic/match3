#include <sstream>
#include <cmath>
#include <algorithm>
#include "DrawBoard.h"
#include "GameConfig.h"
#include "resources.h"

//TODO clean unused variables
DrawBoard::DrawBoard(int render_window_w, int render_window_h)
    : d_columns(GameConfig::GetInstance().GetColSize()),
      d_rows(GameConfig::GetInstance().GetRowSize()),
      d_positionX(0),
      d_positionY(0),
      d_cellWidth(GameConfig::GetInstance().GetCellSize()),
      d_cellHeight(GameConfig::GetInstance().GetCellSize()),
      d_currentMoveCount(GameConfig::GetInstance().GetMoveCount()),
      d_gemPositioningTweakFactor(9.0f),
      d_targetGemSize(0.0f),
      d_state(DrawBoardState::SelectFirstGemState),
      d_selectedCell(0),
      d_isGemSelected(false),
      d_isAnimating(false),
      d_combinedObjectivesGemTexture(std::make_unique<sf::RenderTexture>()),
      d_combinedScoreTexture(std::make_unique<sf::RenderTexture>()),
      d_combinedTileTexture(std::make_unique<sf::RenderTexture>()),
      d_score(std::make_unique<sf::Text>()),
      d_ResolutionW(render_window_w),
      d_ResolutionH(render_window_h)
{
    d_scoreFont.loadFromFile(std::string(PATH_TO_RECOURCES) + "arial.ttf");

    // set the score to display
    d_score->setFont(d_scoreFont);
    d_score->setString(std::to_string(GameConfig::GetInstance().GetMoveCount()));
    d_score->setCharacterSize(90); // in pixels, not points!
    d_score->setLetterSpacing(2);
    d_score->setFillColor(sf::Color::White);
    d_score->setStyle(sf::Text::Bold);


    // create array of gems
    d_gemArray.resize(d_columns * d_rows);

    // initialize board dimensions
    InitBoardDimensions();

    // create scoreboard texture
    GenerateScoreBoardTexture();

    // create objectives texture
    GenerateObjectives();

    // create background board texture
    GenerateTileBoardTexture();
}

void DrawBoard::SetPosition(int x, int y)
{
    d_positionX = x;
    d_positionY = y;
}

constexpr void DrawBoard::InitBoardDimensions()
{
    d_targetGemSize = d_cellWidth * 0.8f;

    // Draw the tiles by calculating their positions
    for (int i = 0; i < d_rows * d_columns; ++i) {
        const int x = i % d_columns;
        const int y = i / d_columns;

        const auto &gem = d_gemArray[i];
        if (gem != nullptr) {
            float cur_gem_scale_factor = d_targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);

            gem->GetSprite()->setPosition(d_positionX + x * d_cellWidth + d_gemPositioningTweakFactor * cur_gem_scale_factor, d_positionY + y * d_cellHeight + d_gemPositioningTweakFactor * cur_gem_scale_factor);
            gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
        }
    }
}

std::shared_ptr<Gem> DrawBoard::GetGem(int column, int row)
{
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        return d_gemArray[row * d_columns + column];
    }

    return nullptr;
}

void DrawBoard::Add(int column, int row, Gem::Color color)
{
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        std::unique_ptr<Gem> gem = std::make_unique<Gem>(color);
        gem->ApplySprite();
        gem->SetState(Gem::State::Normal);

        const int gem_index = row * d_columns + column;

        const int x = gem_index % d_columns;
        const int y = gem_index / d_columns;
        const float cur_gem_scale_factor = d_targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);
        gem->GetSprite()->setPosition(d_positionX + x * d_cellWidth + d_gemPositioningTweakFactor * cur_gem_scale_factor, d_positionY + y * d_cellHeight + d_gemPositioningTweakFactor * cur_gem_scale_factor);
        gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);

        d_gemArray[row * d_columns + column] = std::move(gem);
    }
}

void DrawBoard::MarkRemoved(int column, int row)
{
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        const auto &remove_gem = d_pendingRemovalGemArray.emplace_back(std::move(d_gemArray[row * d_columns + column]));
        remove_gem->SetState(Gem::State::Removed);
        sf::Vector2f scale_to(0.01f, 0.01f);
        d_PlayScaleAnimation(remove_gem, scale_to);
        d_state = DrawBoardState::RemoveGemsState;
    }
}

void DrawBoard::Move(const std::shared_ptr<Gem> &move_gem, int column, int row)
{
    // cell exists, move the gem there
    if (row >= 0 && row < d_rows && column >= 0 && column < d_columns) {
        const int gem_index = row * d_columns + column;
        d_gemArray[gem_index] = move_gem;
        const auto &gem = d_gemArray[gem_index];

        const int x = gem_index % d_columns;
        const int y = gem_index / d_columns;
        const float cur_gem_scale_factor = d_targetGemSize / gem->GetSprite()->getTexture()->getSize().y; //std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);

        const float move_x = d_positionX + x * d_cellWidth + d_gemPositioningTweakFactor * cur_gem_scale_factor;
        const float move_y = d_positionY + y * d_cellHeight + d_gemPositioningTweakFactor * cur_gem_scale_factor;
        const sf::Vector2f move_to(move_x, move_y);
        if (std::floor(gem->GetSprite()->getPosition().x) == std::floor(move_x))
            d_PlayMoveVertAnimation(gem, move_to);
        else
            d_PlayMoveHorizAnimation(gem, move_to);
    }
}

void DrawBoard::UpdateMoveCount(int move_count)
{
    d_currentMoveCount = move_count;
    d_score->setString(std::to_string(d_currentMoveCount));
}

void DrawBoard::UpdateObjectives(const std::map<int, int> &objectives)
{
    for (const auto &[color, val] : objectives) {
        d_currentObjectives[Gem::Color(color)].setString(std::to_string(val));
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

void DrawBoard::ScaleAnimationHandler(std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> handlerFunction)
{
    d_PlayScaleAnimation = std::move(handlerFunction);
}

void DrawBoard::MoveHorizAnimationHandler(std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> handlerFunction)
{
    d_PlayMoveHorizAnimation = std::move(handlerFunction);
}

void DrawBoard::MoveVertAnimationHandler(std::function<void(const std::shared_ptr<Gem> &, const sf::Vector2f &)> handlerFunction)
{
    d_PlayMoveVertAnimation = std::move(handlerFunction);
}

void DrawBoard::OnMousePress(sf::Event *event)
{
    // check if mouse is over board
    if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left && !d_isGemSelected && !d_isAnimating) {
        // find the cell where the user clicked
        const int pos_adjusted_x = event->mouseButton.x - d_positionX;
        const int pos_adjusted_y = event->mouseButton.y - d_positionY;

        if (pos_adjusted_x < 0 || pos_adjusted_y < 0)
            return;

        const int x_cell = (event->mouseButton.x - d_positionX) / d_cellWidth;
        const int y_cell = (event->mouseButton.y - d_positionY) / d_cellHeight;

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
        const int second_cell = y_cell * d_columns + x_cell;

        // user selected the second gem
        if (d_state == DrawBoardState::SelectSecondGemState) {
            // invoke gems selected event
            SwapCells(d_selectedCell, second_cell);

            const auto &gem = d_gemArray[d_selectedCell];
            gem->SetState(Gem::State::Normal);
            d_state = DrawBoardState::SelectFirstGemState;

            const float cur_gem_scale_factor = d_targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x, gem->GetSprite()->getTexture()->getSize().y);
            d_PlayScaleAnimation(gem, sf::Vector2f(cur_gem_scale_factor, cur_gem_scale_factor));
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

void DrawBoard::GenerateObjectives()
{
    float total_width = 0;
    float max_height = 0;
    float offset_x = 10.0f;
    const float offset_y = 15.0f;
    const float h_spacing = 10.0f;
    const float v_spacing = 30.0f;
    const float scaling = 0.9f;
    for (const auto &[key, text] : GameConfig::GetInstance().GetObjectives()) {
        const auto &gem_sprite = Gem::GetGemSpriteMap().at(Gem::Color(key));
        if (gem_sprite.getLocalBounds().height > max_height)
            max_height = gem_sprite.getLocalBounds().height;

        total_width += gem_sprite.getGlobalBounds().width + h_spacing;
    }

    max_height += offset_y;
    d_combinedObjectivesGemTexture->create(total_width, max_height);

    // set initial objectives to display
    for (const auto [key, value] : GameConfig::GetInstance().GetObjectives()) {
        sf::Text objective_text;
        objective_text.setFont(d_scoreFont);
        objective_text.setString(std::to_string(value));
        objective_text.setCharacterSize(50);
        objective_text.setFillColor(sf::Color::White);
        objective_text.setScale(scaling, scaling);
        sf::Sprite objective_sprite = Gem::GetGemSpriteMap().at(Gem::Color(key));
        objective_sprite.setPosition(offset_x, offset_y); // Adjust vertical position as needed
        objective_sprite.setScale(scaling, scaling);

        // Get the bounding box of the sprite
        sf::FloatRect sprite_g_bbox = objective_sprite.getGlobalBounds();

        // Get the bounding box of the text
        sf::FloatRect text_l_bbox = objective_text.getLocalBounds();

        // Set the origin of the text to its center bottom
        objective_text.setOrigin(text_l_bbox.left + text_l_bbox.width / 2.0f,
                                 text_l_bbox.top + text_l_bbox.height);

        // Set the position of the text to be centered below the sprite
        objective_text.setPosition(sprite_g_bbox.left + sprite_g_bbox.width / 2.0f,
                                   sprite_g_bbox.top + max_height + v_spacing);


        d_combinedObjectivesGemTexture->draw(objective_sprite);
        d_currentObjectives[Gem::Color(key)] = std::move(objective_text);
        offset_x += objective_sprite.getGlobalBounds().width + h_spacing; // Move to the next position
    }
    d_combinedObjectivesGemTexture->display();
}

void DrawBoard::GenerateScoreBoardTexture() const
{
    const sf::Texture &center_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_2.png");
    sf::Sprite center_sprite(center_texture);

    const auto cent_texture_w = center_texture.getSize().x;
    const auto cent_texture_h = center_texture.getSize().y;
    const auto cent_texture_scale_factor_x = static_cast<float>(d_cellWidth) / cent_texture_w;
    const auto cent_texture_scale_factor_y = static_cast<float>(d_cellHeight) / cent_texture_h;
    center_sprite.setScale(cent_texture_scale_factor_x, cent_texture_scale_factor_y);

    const sf::Texture &borders_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_1.png");
    sf::Sprite borders_sprite(borders_texture);
    const auto bord_texture_w = borders_texture.getSize().x;
    const auto bord_texture_h = borders_texture.getSize().y;
    const auto bord_texture_scale_factor_x = static_cast<float>(d_cellWidth) / bord_texture_w;
    const auto bord_texture_scale_factor_y = static_cast<float>(d_cellHeight) / bord_texture_h;
    borders_sprite.setScale(bord_texture_scale_factor_x, bord_texture_scale_factor_y);

    const int render_window_width = d_ResolutionW;
    int num_of_tiles = ceil(static_cast<float>(render_window_width) / d_cellWidth);
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
    const sf::Texture &tile1_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_1.png");
    sf::Sprite tile1_sprite(tile1_texture);
    const auto tile1_text_size_w = tile1_sprite.getTexture()->getSize().x;
    const auto tile1_text_size_h = tile1_sprite.getTexture()->getSize().y;

    // scale tile1 size to cell size
    float scale_factor_x = static_cast<float>(d_cellWidth) / tile1_text_size_w;
    float scale_factor_y = static_cast<float>(d_cellHeight) / tile1_text_size_h;
    tile1_sprite.setScale(scale_factor_x, scale_factor_y);

    const sf::Texture &tile2_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_2.png");

    sf::Sprite tile2_sprite(tile2_texture);
    const auto tile2_text_size_w = tile2_sprite.getTexture()->getSize().x;
    const auto tile2_text_size_h = tile2_sprite.getTexture()->getSize().y;

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
        seq = (d_rows % 2 != 0) == seq;
    }

    d_combinedTileTexture->display();
}

void DrawBoard::ClearPendingRemovalGems()
{
    d_pendingRemovalGemArray.clear();
}

void DrawBoard::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::Sprite score_board(d_combinedScoreTexture->getTexture());

    const float score_board_scale_factor = static_cast<float>(target.getSize().x) / d_combinedScoreTexture->getSize().x;
    score_board.setScale(score_board_scale_factor, score_board_scale_factor);
    score_board.setPosition(0, 0);

    target.draw(score_board);
    auto score_text_l_bbox = d_score->getLocalBounds();
    d_score->setOrigin(score_text_l_bbox.left + score_text_l_bbox.width/2.0f, score_text_l_bbox.top + score_text_l_bbox.height/2.0f);
    auto score_board_g_bbox = score_board.getGlobalBounds();
    d_score->setPosition(score_board_g_bbox.left + score_board_g_bbox.width/2.0f, score_board_g_bbox.top + score_board_g_bbox.height/2.0f);
    target.draw(*d_score);

    sf::Sprite obj_gem_sprite(d_combinedObjectivesGemTexture->getTexture());
    target.draw(obj_gem_sprite);
    for (const auto &p : d_currentObjectives) {
        target.draw(p.second);
    }

    sf::Sprite board(d_combinedTileTexture->getTexture());
    board.setPosition(d_positionX, d_positionY);
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
