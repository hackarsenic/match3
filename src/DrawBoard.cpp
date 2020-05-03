#include <sstream>
#include <cmath>  
#include "DrawBoard.hpp"

DrawBoard::DrawBoard(int columns, int rows) :

	_Columns(columns),
	_Rows(rows),
	_PositionX(0),
	_PositionY(0),
	_Width(100),
	_Height(100),
	_CellWidth(0),
	_CellHeight(0),
	_MousePositionX(0),
	_MousePositionY(0),
	_State(DrawBoardState::SelectFirstGemState),
	_IsGemSelected(false)
{
	// create array of gems
	_GemArray.resize(_Columns * _Rows, nullptr);

	// initialize board dimensions
	InitBoardDimentions();
	
	// create background board texture
	GenerateTileBoardTexture();
}

DrawBoard::~DrawBoard()
{
	// clear the vector
	for(auto i : _GemArray)
	{
		delete i;
	}
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

void DrawBoard::InitBoardDimentions()
{
	_CellWidth = 110;
	_CellHeight = 110;

	if (_Rows >= _Columns)
	{
		_CellWidth = _CellWidth - (5 * _Rows);
		_CellHeight = _CellHeight - (5 * _Rows);
	}
	else
	{
		_CellWidth = _CellWidth - (5 * _Columns);
		_CellHeight = _CellHeight - (5 * _Columns);
	}
		
	SetWidth(_CellWidth * _Columns);
	SetHeight(_CellHeight * _Rows);
}

Gem* DrawBoard::GetGem(int column, int row) const
{
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns)
	{
		return _GemArray[row * _Columns + column];
	}

	return nullptr;
}

void DrawBoard::Add(int column, int row, Gem::Color color)
{
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns)
	{
		Gem *gem = new Gem(color);
		gem->ApplySprite();
		gem->SetState(Gem::State::Normal);

		_GemArray[row * _Columns + column] = gem;
	}
}

void DrawBoard::Remove(int column, int row)
{
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns)
	{
		Gem* gem = _GemArray[row * _Columns + column];

		// remove gem
		delete gem;
		_GemArray[row * _Columns + column] = nullptr;
	}
}

void DrawBoard::Move(Gem* gem, int column, int row)
{
	// cell exists, move the gem there
	if (row >= 0 && row < _Rows && column >= 0 && column < _Columns)
	{
		_GemArray[row * _Columns + column] = gem;
	}
}


void DrawBoard::GemsSelectedHandler(std::function<void(int, int, int, int)> handlerFunction)
{
	Gems_Selected = handlerFunction;
}

void DrawBoard::BombSelectedHandler(std::function<bool(int, int)> handlerFunction)
{
	Bomb_Selected = handlerFunction;
}

void DrawBoard::OnMousePress(sf::Event *event)
{
	// check if mouse is over board
	if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left && !_IsGemSelected)
	{
		// find the cell where the user clicked
		int x_cell = (event->mouseButton.x - _PositionX) / _CellWidth;
		int y_cell = (event->mouseButton.y - _PositionY) / _CellHeight;

		if (x_cell > _Columns - 1)
			x_cell = _Columns - 1;
		if (y_cell > _Rows - 1)
			y_cell = _Rows - 1;

		if (_State == DrawBoardState::SelectFirstGemState)
		{
			_SelectedCell = y_cell * _Columns + x_cell;

			_GemArray[_SelectedCell]->SetState(Gem::State::Selected);

			if(DetonateBomb(_SelectedCell))
				return;
			
			_State = DrawBoardState::SelectSecondGemState;
		}

		_IsGemSelected = true;
		return;
	}

	if (event->type == event->MouseButtonPressed && event->mouseButton.button == sf::Mouse::Left && _IsGemSelected)
	{
		// find the second cell where user clicked
		int x_cell = (event->mouseButton.x - _PositionX) / _CellWidth;
		int y_cell = (event->mouseButton.y - _PositionY) / _CellHeight;

		if (x_cell > _Columns - 1) x_cell = _Columns - 1;
		if (y_cell > _Rows - 1) y_cell = _Rows - 1;

		// second cell where user clicked
		int second_cell = y_cell * _Columns + x_cell;

		// user selected the second gem
		if (_State == DrawBoardState::SelectSecondGemState)
		{
			// invoke gems selected event
			SwapCells(_SelectedCell, second_cell);

			_GemArray[_SelectedCell]->SetState(Gem::State::Normal);
			_State = DrawBoardState::SelectFirstGemState;

		}

		// user clicked same cell twice
		if (_State == DrawBoardState::SelectSecondGemState && _SelectedCell == second_cell)
		{
			_GemArray[second_cell]->SetState(Gem::State::Selected);
		}

		_IsGemSelected = false;
		return;
	}
}

void DrawBoard::SwapCells(int first_cell, int second_cell)
{

	// swap if cells are adjacent
	if (((first_cell / _Columns) == (second_cell / _Columns) && abs(first_cell - second_cell) == 1)
		|| abs(first_cell - second_cell) == _Columns)
	{
		// invoke gems selected event
		Gems_Selected(_SelectedCell % _Columns, _SelectedCell / _Columns, second_cell % _Columns, second_cell / _Columns);
	}
}

bool DrawBoard::DetonateBomb(int cell)
{
	return Bomb_Selected(_SelectedCell % _Columns, _SelectedCell / _Columns);
}

void DrawBoard::GenerateTileBoardTexture()
{
	sf::Texture &tile1_texture = ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/tile_1.png");
	sf::Sprite tile1_sprite(tile1_texture);
	int tile1_text_size_w = tile1_sprite.getTexture()->getSize().x;
	int tile1_text_size_h = tile1_sprite.getTexture()->getSize().y;

	// scale tile1 size to cell size
	float scale_factor_x = static_cast<float> (_CellWidth) / tile1_text_size_w;
	float scale_factor_y = static_cast<float> (_CellHeight) / tile1_text_size_h;
	tile1_sprite.setScale(scale_factor_x, scale_factor_y);

	sf::Texture &tile2_texture = ResourceManager::GetTexture("C:/Users/User/Desktop/DevTestGame-master/resources/tile_2.png");;
	sf::Sprite tile2_sprite(tile2_texture);
	int tile2_text_size_w = tile2_sprite.getTexture()->getSize().x;
	int tile2_text_size_h = tile2_sprite.getTexture()->getSize().y;

	// scale tile2 size to cell size
	scale_factor_x = static_cast<float> (_CellWidth) / tile2_text_size_w;
	scale_factor_y = static_cast<float> (_CellHeight) / tile2_text_size_h;
	tile2_sprite.setScale(scale_factor_x, scale_factor_y);
	
	bool seq = true;
	_CombinedTileTexture.create(_Columns*tile1_text_size_w, _Rows*tile1_text_size_h);

	// populate the vector, with two tile sprites 
	for (int i = 0; i < _Columns; ++i) {
		for (int j = 0; j < _Rows; ++j)
		{
			if (seq)
			{
				tile1_sprite.setPosition(i * tile1_text_size_w * scale_factor_x, j * tile1_text_size_h * scale_factor_y);
				_CombinedTileTexture.draw(tile1_sprite);
			}
			else
			{
				tile2_sprite.setPosition(i * tile2_text_size_w * scale_factor_x, j * tile2_text_size_h * scale_factor_y);
				_CombinedTileTexture.draw(tile2_sprite);

			}
			seq = !seq;
		}
		seq = (_Rows % 2) ? seq : !seq;
	}

	_CombinedTileTexture.display();
}

sf::RenderTexture &DrawBoard::GetTileBoardTexture()
{
	return _CombinedTileTexture;
}


void DrawBoard::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::Sprite board(_CombinedTileTexture.getTexture());
	board.setPosition(_PositionX, _PositionY);
	float scale_factor_x = static_cast<float> ((_CellWidth * _Rows)) / _CombinedTileTexture.getTexture().getSize().x;
	float scale_factor_y = static_cast<float> ((_CellHeight * _Rows)) / _CombinedTileTexture.getTexture().getSize().y;
	//board.setScale(scale_factor_x, scale_factor_y);
	target.draw(board);

	int gem_positioning_tweak_factor = 5;
	float gem_scaling_tweak_factor = 0.7;
	//Draw the tiles by calculating their positions
	for (int i = 0; i < _Rows * _Columns; ++i)
	{
		int x = i % _Columns;
		int y = i / _Columns;

		Gem* gem = _GemArray[i];
		if (gem != nullptr)
		{
			sf::Sprite* currentSprite = gem->GetSprite();

			// draw normal state
			if (gem->GetState() == Gem::State::Normal)
			{
				currentSprite->setPosition(_PositionX + x * _CellWidth + gem_positioning_tweak_factor, _PositionY + y * _CellHeight + gem_positioning_tweak_factor);
				currentSprite->setScale(gem_scaling_tweak_factor, gem_scaling_tweak_factor);
				target.draw(*gem->GetSprite());
			}

			// draw selected state
			if (gem->GetState() == Gem::State::Selected)
			{
				currentSprite->setPosition(_PositionX + x * _CellWidth + gem_positioning_tweak_factor, _PositionY + y * _CellHeight + gem_positioning_tweak_factor);
				currentSprite->setScale(gem_scaling_tweak_factor * 0.75, gem_scaling_tweak_factor * 0.75);
				target.draw(*gem->GetSprite());
			}
		}
	}
}
