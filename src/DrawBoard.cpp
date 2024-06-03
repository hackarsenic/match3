#include <sstream>
#include <cmath>
#include <algorithm>
#include "DrawBoard.hpp"
#include "resources.h"

bool DrawBoard::_scFlag = true;
int DrawBoard::_ResolutionW = 0;
int DrawBoard::_ResolutionH = 0;

//sf::RenderTexture DrawBoard::_CombinedScoreTexture;

DrawBoard::DrawBoard(int columns, int rows, int cell_size) :

	_Columns(columns),
	_Rows(rows),
	_PositionX(0),
	_PositionY(0),
	_Width(100),
	_Height(100),
	_CellWidth(cell_size),
	_CellHeight(cell_size),
	_MousePositionX(0),
	_MousePositionY(0),
	_gemPositioningTweakFactor(13.0f),
	_targetGemSize(0.0f),
	_State(DrawBoardState::SelectFirstGemState),
	_IsGemSelected(false),
    _CombinedScoreTexture(std::make_unique<sf::RenderTexture>()),
    _isAnimating(false)
{
	// create array of gems
	_GemArray.resize(_Columns * _Rows, nullptr);

	// initialize board dimensions
    InitBoardDimensions();
	
	// create background board texture
	GenerateTileBoardTexture();
}


DrawBoard::~DrawBoard()
{
	// clear the vector
//	for (auto i : _GemArray) {
//		delete i;
//	}
//
//	for (auto i : _pendingRemovalGemArray) {
//	    delete i;
//	}
}

void DrawBoard::SetPosition(int x, int y)
{
	_PositionX = x;
	_PositionY = y;
}

void DrawBoard::SetWidth(int width)
{
	_Width = width;
}

void DrawBoard::SetHeight(int height)
{
	_Height = height;
}

void DrawBoard::InitBoardDimensions()
{
	if (_Rows >= _Columns) {
		_CellWidth = _CellWidth - (5 * _Rows);
		_CellHeight = _CellHeight - (5 * _Rows);
	}
	else {
		_CellWidth = _CellWidth - (5 * _Columns);
		_CellHeight = _CellHeight - (5 * _Columns);
	}
		
	SetWidth(_CellWidth * _Columns);
	SetHeight(_CellHeight * _Rows);

    _targetGemSize = _CellWidth * 0.7;

    //Draw the tiles by calculating their positions
    for (int i = 0; i < _Rows * _Columns; ++i) {
        int x = i % _Columns;
        int y = i / _Columns;

        const auto &gem = _GemArray[i];
        if (gem != nullptr) {
            const auto &currentSprite = gem->GetSprite();

            float cur_gem_scale_factor = _targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x,
                                                                    gem->GetSprite()->getTexture()->getSize().y);

            gem->GetSprite()->setPosition(_PositionX + x * _CellWidth + _gemPositioningTweakFactor * cur_gem_scale_factor,
                                          _PositionY + y * _CellHeight + _gemPositioningTweakFactor * cur_gem_scale_factor);
            gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
        }
    }

}

Gem *DrawBoard::GetGem(int column, int row) const
{
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns) {
		return _GemArray[row * _Columns + column];
	}

	return nullptr;
}

void DrawBoard::Add(int column, int row, Gem::Color color)
{
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns) {
		Gem *gem = new Gem(color);
		gem->ApplySprite();
		gem->SetState(Gem::State::Normal);

		int gem_index = row * _Columns + column;
		_GemArray[row * _Columns + column] = gem;

        int x = gem_index % _Columns;
        int y = gem_index / _Columns;
        float gem_positioning_tweak_factor = 13.0f;
        float cur_gem_scale_factor = _targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x,
                                                                gem->GetSprite()->getTexture()->getSize().y);
//        gem->SetDefaultScale(cur_gem_scale_factor, cur_gem_scale_factor);
//        auto currentSprite = gem->GetSprite();
        gem->GetSprite()->setPosition(_PositionX + x * _CellWidth + gem_positioning_tweak_factor * cur_gem_scale_factor,
                                      _PositionY + y * _CellHeight + gem_positioning_tweak_factor * cur_gem_scale_factor);
        gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
	}
}


void DrawBoard::Remove(int column, int row)
{
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns) {
		Gem* gem = _GemArray[row * _Columns + column];
		gem->SetState(Gem::State::Removed);

		// remove gem
		delete gem;

		_GemArray[row * _Columns + column] = nullptr;
	}

	_State = DrawBoardState::RemoveGemsState;
}

//TODO maybe write separate swap func which will handle unique_ptrs
void DrawBoard::Move(Gem *gem, int column, int row)
{
	// cell exists, move the gem there
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns) {
        int gem_index = row * _Columns + column;
        _GemArray[row * _Columns + column] = gem;

        int x = gem_index % _Columns;
        int y = gem_index / _Columns;
        float cur_gem_scale_factor = _targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x,
                                                                gem->GetSprite()->getTexture()->getSize().y);
//        std::cout << "Current: " << gem->GetSprite()->getPosition().x << " " << gem->GetSprite()->getPosition().y << std::endl;
//        std::cout << "Target: " << _PositionX + x * _CellWidth + _gemPositioningTweakFactor * cur_gem_scale_factor << " " << _PositionY + y * _CellHeight + _gemPositioningTweakFactor * cur_gem_scale_factor << std::endl;


        float move_x = _PositionX + x * _CellWidth + _gemPositioningTweakFactor * cur_gem_scale_factor;
        float move_y = _PositionY + y * _CellHeight + _gemPositioningTweakFactor * cur_gem_scale_factor;
        sf::Vector2f move_to(move_x, move_y);
        if (std::floor(gem->GetSprite()->getPosition().x) == std::floor(move_x))
            PlayMoveVertAnimation(gem, move_to);
        else
            PlayMoveHorizAnimation(gem, move_to);
//        gem->GetSprite()->setPosition(_PositionX + x * _CellWidth + _gemPositioningTweakFactor * cur_gem_scale_factor,
//                                      _PositionY + y * _CellHeight + _gemPositioningTweakFactor * cur_gem_scale_factor);
//        gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
	}
}


void DrawBoard::GemsSelectedHandler(std::function<void(int, int, int, int)> handlerFunction)
{
	Gems_Selected = std::move(handlerFunction);
}

void DrawBoard::BombSelectedHandler(std::function<bool(int, int)> handlerFunction)
{
	Bomb_Selected = std::move(handlerFunction);
}

void DrawBoard::ScaleAnimationHandler(std::function<void(Gem *, const sf::Vector2f &)> handlerFunction)
{
    PlayScaleAnimation = std::move(handlerFunction);
}

void DrawBoard::MoveHorizAnimationHandler(std::function<void(Gem *, const sf::Vector2f &)> handlerFunction)
{
    PlayMoveHorizAnimation = std::move(handlerFunction);
}

void DrawBoard::MoveVertAnimationHandler(std::function<void(Gem *, const sf::Vector2f &)> handlerFunction)
{
    PlayMoveVertAnimation = std::move(handlerFunction);
}

void DrawBoard::OnMousePress(sf::Event *event)
{
	// check if mouse is over board
	if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left && !_IsGemSelected && !_isAnimating) {
		// find the cell where the user clicked
		int pos_adjusted_x = event->mouseButton.x - _PositionX;
		int pos_adjusted_y = event->mouseButton.y - _PositionY;

		if (pos_adjusted_x < 0 || pos_adjusted_y < 0)
            return;

		int x_cell = (event->mouseButton.x - _PositionX) / _CellWidth;
		int y_cell = (event->mouseButton.y - _PositionY) / _CellHeight;


		if (x_cell > _Columns - 1)
            return;
		if (y_cell > _Rows - 1)
            return;

		if (_State == DrawBoardState::SelectFirstGemState || _State == DrawBoardState::RemoveGemsState)	{
			_SelectedCell = y_cell * _Columns + x_cell;

			_GemArray[_SelectedCell]->SetState(Gem::State::Selected);

			if(DetonateBomb(_SelectedCell))
				return;
			
			_State = DrawBoardState::SelectSecondGemState;

			const auto &gem = _GemArray[_SelectedCell];
			const auto &current_sprite = gem->GetSprite();
			float cur_gem_scale_factor = current_sprite->getScale().x;
			sf::Vector2f scale_to(current_sprite->getScale().x * 0.75, current_sprite->getScale().x * 0.75);
            PlayScaleAnimation(gem, scale_to);
//            current_sprite->setScale(cur_gem_scale_factor * 0.75, cur_gem_scale_factor * 0.75);
		}

		_IsGemSelected = true;
		return;
	}

	if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left && _IsGemSelected)	{
		// find the second cell where user clicked
		int x_cell = (event->mouseButton.x - _PositionX) / _CellWidth;
		int y_cell = (event->mouseButton.y - _PositionY) / _CellHeight;

		if (x_cell > _Columns - 1) x_cell = _Columns - 1;
		if (y_cell > _Rows - 1) y_cell = _Rows - 1;

		// second cell where user clicked
		int second_cell = y_cell * _Columns + x_cell;

		// user selected the second gem
		if (_State == DrawBoardState::SelectSecondGemState) {
			// invoke gems selected event
			SwapCells(_SelectedCell, second_cell);

            auto gem = _GemArray[_SelectedCell];
            gem->SetState(Gem::State::Normal);
			_State = DrawBoardState::SelectFirstGemState;

            float target_gem_size = _CellWidth * 0.7f;
            float cur_gem_scale_factor = _targetGemSize / std::max(gem->GetSprite()->getTexture()->getSize().x,
                                                                    gem->GetSprite()->getTexture()->getSize().y);
            PlayScaleAnimation(gem, sf::Vector2f(cur_gem_scale_factor, cur_gem_scale_factor));
//            current_sprite->setScale(cur_gem_scale_factor, cur_gem_scale_factor);

		}

		// user clicked same cell twice
		if (_State == DrawBoardState::SelectSecondGemState && _SelectedCell == second_cell) {
		    auto gem = _GemArray[second_cell];
            gem->SetState(Gem::State::Selected);

            float target_gem_size = _CellWidth * 0.7f;
            float cur_gem_scale_factor = target_gem_size / std::max(gem->GetSprite()->getTexture()->getSize().x,
                                                                    gem->GetSprite()->getTexture()->getSize().y);
            gem->GetSprite()->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
//            auto current_sprite = gem->GetSprite();
//            current_sprite->setScale(cur_gem_scale_factor, cur_gem_scale_factor);
		}

		_IsGemSelected = false;
		return;
	}
}

void DrawBoard::SwapCells(int first_cell, int second_cell)
{
	// swap if cells are adjacent
	if (((first_cell / _Columns) == (second_cell / _Columns) && abs(first_cell - second_cell) == 1)
		|| abs(first_cell - second_cell) == _Columns) {
		// invoke gems selected event
		Gems_Selected(_SelectedCell % _Columns, _SelectedCell / _Columns, second_cell % _Columns, second_cell / _Columns);
	}
}

bool DrawBoard::DetonateBomb(int cell)
{
	return Bomb_Selected(_SelectedCell % _Columns, _SelectedCell / _Columns);
}


//TODO finish score board
void DrawBoard::GenerateScoreBoardTexture() const
{
    sf::Texture &center_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_2.png");
    sf::Sprite center_sprite(center_texture);
    int cent_texture_w = center_texture.getSize().x;
    int cent_texture_h = center_texture.getSize().y;
    float cent_texture_scale_factor_x = static_cast<float>(_CellWidth) / cent_texture_w;
    float cent_texture_scale_factor_y = static_cast<float>(_CellHeight) / cent_texture_h;
    center_sprite.setScale(cent_texture_scale_factor_x, cent_texture_scale_factor_y);

    sf::Texture &borders_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_1.png");
    sf::Sprite borders_sprite(borders_texture);
    int bord_texture_w = borders_texture.getSize().x;
    int bord_texture_h = borders_texture.getSize().y;
    float bord_texture_scale_factor_x = static_cast<float> (_CellWidth) / bord_texture_w;
    float bord_texture_scale_factor_y = static_cast<float> (_CellHeight) / bord_texture_h;
    borders_sprite.setScale(bord_texture_scale_factor_x, bord_texture_scale_factor_y);

    int render_window_width = _ResolutionW;
    int num_of_tiles = ceil(static_cast<float>(render_window_width) / (_CellWidth));
    if (num_of_tiles%2 != 0) 
        ++num_of_tiles;

    _CombinedScoreTexture->create(num_of_tiles * cent_texture_w * cent_texture_scale_factor_x, 2 * cent_texture_h * cent_texture_scale_factor_y);

    // need only two rows
    for (int i = 0; i < num_of_tiles; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (i == num_of_tiles/2 || i == (num_of_tiles/2 - 1)) {
                center_sprite.setPosition(i * cent_texture_w * cent_texture_scale_factor_x, j * cent_texture_h * cent_texture_scale_factor_y);
                _CombinedScoreTexture->draw(center_sprite);
            } else {
                borders_sprite.setPosition(i * bord_texture_w * bord_texture_scale_factor_x, j * bord_texture_h * bord_texture_scale_factor_y);
                _CombinedScoreTexture->draw(borders_sprite);
            }
        }
    }

    _CombinedScoreTexture->display();
}

void DrawBoard::GenerateTileBoardTexture()
{
	sf::Texture &tile1_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_1.png");
	sf::Sprite tile1_sprite(tile1_texture);
	int tile1_text_size_w = tile1_sprite.getTexture()->getSize().x;
	int tile1_text_size_h = tile1_sprite.getTexture()->getSize().y;

	// scale tile1 size to cell size
	float scale_factor_x = static_cast<float> (_CellWidth) / tile1_text_size_w;
	float scale_factor_y = static_cast<float> (_CellHeight) / tile1_text_size_h;
	tile1_sprite.setScale(scale_factor_x, scale_factor_y);

	sf::Texture &tile2_texture = ResourceManager::GetTexture(std::string(PATH_TO_RECOURCES) + "tile_2.png");;
	sf::Sprite tile2_sprite(tile2_texture);
	int tile2_text_size_w = tile2_sprite.getTexture()->getSize().x;
	int tile2_text_size_h = tile2_sprite.getTexture()->getSize().y;

	// scale tile2 size to cell size
	scale_factor_x = static_cast<float> (_CellWidth) / tile2_text_size_w;
	scale_factor_y = static_cast<float> (_CellHeight) / tile2_text_size_h;
	tile2_sprite.setScale(scale_factor_x, scale_factor_y);
	
	bool seq = true;
	_CombinedTileTexture.create(_Columns*tile1_text_size_w*scale_factor_x, _Rows*tile1_text_size_h*scale_factor_y);

	// populate the vector, with two tile sprites 
	for (int i = 0; i < _Columns; ++i) {
		for (int j = 0; j < _Rows; ++j)	{
			if (seq) {
				tile1_sprite.setPosition(i * tile1_text_size_w * scale_factor_x, j * tile1_text_size_h * scale_factor_y);
				_CombinedTileTexture.draw(tile1_sprite);
			}
			else {
				tile2_sprite.setPosition(i * tile2_text_size_w * scale_factor_x, j * tile2_text_size_h * scale_factor_y);
				_CombinedTileTexture.draw(tile2_sprite);

			}
			seq = !seq;
		}
		seq = _Rows % 2 != 0 == seq;
	}

	_CombinedTileTexture.display();
}

sf::RenderTexture &DrawBoard::GetTileBoardTexture()
{
	return _CombinedTileTexture;
}

void DrawBoard::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (_scFlag) {
        _ResolutionW = target.getSize().x;
        _ResolutionH = target.getSize().x; //TODO why x?
        GenerateScoreBoardTexture();
        _scFlag = false;
    }

    sf::Sprite score_board(_CombinedScoreTexture->getTexture());

    float score_board_scale_factor = static_cast<float>(target.getSize().x) / _CombinedScoreTexture->getSize().x;
    score_board.setScale(score_board_scale_factor, score_board_scale_factor);
    score_board.setPosition(0,0);

    target.draw(score_board);


    sf::Sprite board(_CombinedTileTexture.getTexture());
    unsigned new_pos_y = _PositionY + _CombinedScoreTexture->getTexture().getSize().y * score_board.getScale().y;
    board.setPosition(_PositionX, _PositionY);

    board.setScale(1, 1);
    target.draw(board);

    int gem_positioning_tweak_factor = 13;
    float target_gem_size = _CellWidth * 0.7;

    //Draw the tiles by calculating their positions
    for (int i = 0; i < _Rows * _Columns; ++i) {
        int x = i % _Columns;
        int y = i / _Columns;

        Gem *gem = _GemArray[i];
        if (gem != nullptr) {
            target.draw(*gem->GetSprite());
        }
    }
}
