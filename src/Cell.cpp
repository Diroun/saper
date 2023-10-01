#include <vector>

#include "Cell.hpp"
#include "GetCell.hpp"
#include "Global.hpp"

Cell::Cell(unsigned char i_x, unsigned char i_y) :
	mines_around(0),
	x(i_x),
	y(i_y)
{
	reset();
}

bool Cell::get_is_flagged()
{
	//Возврат если на клетке установлен флаг
	return is_flagged;
}

bool Cell::get_is_mine()
{
	//Под клеткой мина..
	return is_mine;
}

bool Cell::get_is_open()
{
	//Ячейка открыта
	return is_open;
}

bool Cell::open(std::vector<Cell>& i_cells)
{
	//Проверка что ячейка открыта, а открывать дважды нельзя.
	if (0 == is_open)
	{
		is_open = 1;

		//Если вокруг ячейки нет мин вовсе, то она открываем все ячейки рядом с собой вплодь до тех, которые имеют рядом мины.
		if (0 == is_mine && 0 == mines_around)
		{
			for (char a = -1; a < 2; a++)
			{
				for (char b = -1; b < 2; b++)
				{
					//Обозначаем игровое поле
					if ((0 == a && 0 == b) || (0 > a + x || 0 > b + y || COLUMNS == a + x || ROWS == b + y))
					{
						continue;
					}

					//Открытие соседних клеток
					get_cell(a + x, b + y, i_cells)->open(i_cells);
				}
			}
		}

		//Запуск эффекта если клетка = мина
		effect_timer -= is_mine;

		//Узнаем а была ли мина?
		return is_mine;
	}

	return 0;
}

bool Cell::update_effect_timer()
{
	//Эффект кончился
	if (0 < effect_timer)
	{
		//Если нет уменьшаем его длительность
		effect_timer--;

		//Проверка на конец эффекта
		if (0 == effect_timer)
		{
			//Возврат единицы, когда эффект кончился
			return 1;
		}
	}

	return 0;
}

unsigned char Cell::get_effect_timer()
{
	//Длительность эффекта
	return effect_timer;
}

unsigned char Cell::get_mines_around()
{
	//мины рядом
	return mines_around;
}

unsigned char Cell::get_mouse_state()
{
	//Клетка на которую мы жмем мышкой
	return mouse_state;
}

void Cell::count_mines_around(std::vector<Cell>& i_cells)
{
	//Просчитываем сколько мин рядом, а начнем с 0
	mines_around = 0;

	//Проверка, что клетка = мина, а значит нет смысла считать вокруг неё мины.
	if (0 == is_mine)
	{
		for (char a = -1; a < 2; a++)
		{
			for (char b = -1; b < 2; b++)
			{
				if ((0 == a && 0 == b) || (0 > a + x || 0 > b + y || COLUMNS == a + x || ROWS == b + y))
				{
					continue;
				}

				//Если сосед имеет мину
				if (1 == get_cell(a + x, b + y, i_cells)->get_is_mine())
				{
					mines_around++; // увеличиваем значение
				}
			}
		}
	}
}

void Cell::flag()
{
	//Запрет помечать открытую клетку
	if (0 == is_open)
	{
		is_flagged = !is_flagged;
	}
}

void Cell::reset()
{
	//Перезапуск все на 0
	is_flagged = 0;
	is_mine = 0;
	is_open = 0;
	effect_timer = EFFECT_DURATION;
	mouse_state = 0;
}

void Cell::set_effect_timer(unsigned char i_effect_timer) // установка длительности эффекта
{
	effect_timer = i_effect_timer;
}

void Cell::set_mine() //установка мины
{
	is_mine = 1;
}

void Cell::set_mouse_state(unsigned char i_mouse_state) //установка курсора на клетке.
{
	mouse_state = i_mouse_state;
}