#include <random>
#include <SFML/Graphics.hpp>

#include "Cell.hpp"
#include "Field.hpp"
#include "GetCell.hpp"
#include "Global.hpp"


Field::Field() :
	game_over(1),
	random_engine(random_device())
{
	//Добавка клеток в вектор
	for (unsigned char a = 0; a < ROWS; a++)
	{
		for (unsigned char b = 0; b < COLUMNS; b++)
		{
			cells.push_back(Cell(b, a));
		}
	}

	restart();
}

bool Field::effect_over()
{
	//Проверяем каждую клетку
	for (Cell& cell : cells)
	{
		//Если эффект какой-то клетки не закончился
		if (0 < cell.get_effect_timer())
		{
			return 0;
		}
	}

	return 1;
}

char Field::get_game_over()
{
	return game_over;
}

unsigned short Field::get_flags()
{
	//Счетчик клеток с флагом.
	unsigned short total_flags = 0;

	for (Cell& cell : cells)
	{
		total_flags += cell.get_is_flagged();
	}

	return total_flags;
}


void Field::draw(sf::RenderWindow& i_window)
{
	//Отрисовка клеток
	sf::RectangleShape cell_shape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

	sf::Sprite icon_sprite;

	sf::Texture icons_texture;
	icons_texture.loadFromFile("Resources/Images/Icons" + std::to_string(CELL_SIZE) + ".png");

	icon_sprite.setTexture(icons_texture);

	//Проходим по каждой клеточке.
	for (unsigned char a = 0; a < COLUMNS; a++)
	{
		for (unsigned char b = 0; b < ROWS; b++)
		{
			//Меняем положение клетки
			cell_shape.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));

			//Если ячейка открыта
			if (1 == get_cell(a, b, cells)->get_is_open())
			{
				//Она считает сколько возле неё мин
				unsigned char mines_around = get_cell(a, b, cells)->get_mines_around();

				cell_shape.setFillColor(sf::Color(146, 182, 255));

				i_window.draw(cell_shape);

				//Если вокруг ячейки имеется мина
				if (0 < mines_around)
				{
					//Тогда мы пользуемся текстуркой и накладываем на ячейку цифру с минами
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					icon_sprite.setTextureRect(sf::IntRect(CELL_SIZE * mines_around, 0, CELL_SIZE, CELL_SIZE));

					i_window.draw(icon_sprite);
				}
			}
			else 
			{
				cell_shape.setFillColor(sf::Color(0, 73, 55));

				//Тут можно корректировать цвет наших клеток
				//Мы не меняем цвет только если игра закончена
				if (0 == game_over)
				{
					if (1 == get_cell(a, b, cells)->get_mouse_state())
					{
						cell_shape.setFillColor(sf::Color(0, 109, 55));
					}
					else if (2 == get_cell(a, b, cells)->get_mouse_state())
					{
						cell_shape.setFillColor(sf::Color(0, 36, 55));
					}
				}

				i_window.draw(cell_shape);

				//Если клетка помечена флагом
				if (1 == get_cell(a, b, cells)->get_is_flagged())
				{
					//Рисуем картинку флага на клетке
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					icon_sprite.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE));

					i_window.draw(icon_sprite);
				}
			}

			get_cell(a, b, cells)->set_mouse_state(0);


			if (0 != game_over && EFFECT_DURATION > get_cell(a, b, cells)->get_effect_timer())
			{
				//Тут просчитывается размер эффектов, допустим взрыв, когда все закрашивается 
				unsigned char effect_size = static_cast<unsigned char>(2 * round(0.5f * CELL_SIZE * ((EFFECT_DURATION - get_cell(a, b, cells)->get_effect_timer()) / static_cast<float>(EFFECT_DURATION))));

				//Таймер эффекта на каждой ячейке имеет рандомную длительность
				std::uniform_int_distribution<unsigned short> effect_duration_distribution(1, EFFECT_DURATION - 1);

				cell_shape.setPosition(floor(CELL_SIZE * (0.5f + a) - 0.5f * effect_size), floor(CELL_SIZE * (0.5f + b) - 0.5f * effect_size));
				cell_shape.setSize(sf::Vector2f(effect_size, effect_size));

				//А цвет взрыва или победы должен быть разным
				if (-1 == game_over)
				{
					cell_shape.setFillColor(sf::Color(255, 36, 0));
				}
				else
				{
					cell_shape.setFillColor(sf::Color(255, 255, 255));
				}

				//Теперь нужно нарисовать сами эффекты
				i_window.draw(cell_shape);

				cell_shape.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

				//Если эффект закончился
				if (1 == get_cell(a, b, cells)->update_effect_timer())
				{
					//То он передается соседу!
					if (0 <= a - 1 && EFFECT_DURATION == get_cell(a - 1, b, cells)->get_effect_timer())
					{
						get_cell(a - 1, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (0 <= b - 1 && EFFECT_DURATION == get_cell(a, b - 1, cells)->get_effect_timer())
					{
						get_cell(a, b - 1, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (COLUMNS > 1 + a && EFFECT_DURATION == get_cell(1 + a, b, cells)->get_effect_timer())
					{
						get_cell(1 + a, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (ROWS > 1 + b && EFFECT_DURATION == get_cell(a, 1 + b, cells)->get_effect_timer())
					{
						get_cell(a, 1 + b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}
				}
			}
		}
	}
}

void Field::flag_cell(unsigned char i_x, unsigned char i_y)
{
	//Запрет ставить флаги, когда игра кончилась
	if (0 == game_over)
	{
		get_cell(i_x, i_y, cells)->flag();
	}
}

void Field::open_cell(unsigned char i_x, unsigned char i_y)
{
	//Здесь мы сначала даем пользователю открыть ячейку, а потом загенерировать мины на поле, чтобы игрок не имел шанс взорваться на старте :)
	if (0 == first_click)
	{
		//Объявим распределение координат
		std::uniform_int_distribution<unsigned short> x_distribution(0, COLUMNS - 1);
		std::uniform_int_distribution<unsigned short> y_distribution(0, ROWS - 1);

		first_click = 1;

		//А теперь загенерируем минное поле!
		for (unsigned short a = 0; a < MINES; a++)
		{
			unsigned char mine_x = static_cast<unsigned char>(x_distribution(random_engine));
			unsigned char mine_y = static_cast<unsigned char>(y_distribution(random_engine));

			//Если в этой ячейке есть мина
			if (1 == get_cell(mine_x, mine_y, cells)->get_is_mine() || (i_x == mine_x && i_y == mine_y))
			{
				//Начинаем снова.
				a--;
			}
			else
			{
				//В другом случае расставляем мины
				get_cell(mine_x, mine_y, cells)->set_mine();
			}
		}

		//Теперь каждая ячейка должна посчитать мины вокруг себя.
		for (Cell& cell : cells)
		{
			cell.count_mines_around(cells);
		}
	}

	//Ячейки не открываются если игра завершена или стоит флаг
	if (0 == game_over && 0 == get_cell(i_x, i_y, cells)->get_is_flagged())
	{
		if (1 == get_cell(i_x, i_y, cells)->open(cells))
		{
			//Если игрок взорвался то статус конца игры ставится на -1 (проигрыш)
			game_over = -1;
		}
		else
		{
			unsigned short total_closed_cells = 0;

			//Считаем сколько ячеек закрыто
			for (Cell& cell : cells)
			{
				total_closed_cells += 1 - cell.get_is_open();
			}

			//Если количество открытых ячеек соответствует общему количеству мин, то игра выйграна
			if (MINES == total_closed_cells)
			{
				//Победа статус = 1
				game_over = 1;

				//Эффект покраски экрана.
				get_cell(i_x, i_y, cells)->set_effect_timer(EFFECT_DURATION - 1);
			}
		}
	}
}

void Field::restart()
{
	//Мы можем перезапускать нашу игру, если она закончена победой или проигрышем
	if (0 != game_over)
	{
		first_click = 0;

		game_over = 0;

		for (Cell& cell : cells)
		{
			cell.reset();
		}
	}
}

//Этой функцией мы можем вызвать функцию клетки
void Field::set_mouse_state(unsigned char i_mouse_state, unsigned char i_x, unsigned char i_y)
{
	get_cell(i_x, i_y, cells)->set_mouse_state(i_mouse_state);
}