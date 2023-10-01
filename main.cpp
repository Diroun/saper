#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>

#include "Cell.hpp"
#include "DrawText.hpp"
#include "Field.hpp"
#include "GetCell.hpp"
#include "Global.hpp"

int main()
{
	//независимость игры от частоты кадров
	unsigned lag = 0;

	//Смайлик имеет 4 образа:
	//0) Ожидание
	//1) Открываем клетку
	//2) Когда мы проиграли :(
	//3) Когда мы выйграли! :)
	unsigned char Smile_state = 0;

	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	sf::Event event;

	//окошко для SFML
	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * COLUMNS * SCREEN_RESIZE, SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * COLUMNS)), "SAPER!", sf::Style::Close);
	//Меняем размер окна
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * COLUMNS, FONT_HEIGHT + CELL_SIZE * ROWS)));

	//картинка смайлика.
	sf::Sprite Smile;

	//текстурка смайлика.
	sf::Texture Smile_texture;
	//Подргружаем текстурку
	Smile_texture.loadFromFile("Resources/Images/Smile.png");

	//Помещаем смайлика в середину вниз
	Smile.setPosition(static_cast<float>(CELL_SIZE * COLUMNS - Smile_texture.getSize().y)/2, CELL_SIZE * ROWS);
	Smile.setTexture(Smile_texture);

	//Поле
	Field field;

	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		//Тут высчитывается задержка
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

		lag += delta_time;

		previous_time += std::chrono::microseconds(delta_time);

		//Пока задержка превышает максимально допустимую длительность кадра
		while (FRAME_DURATION <= lag)
		{
			//Здесь мы можем получить координату клетки, которая находится под курсором во время клика
			unsigned char mouse_cell_x = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).x / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, COLUMNS - 1);
			unsigned char mouse_cell_y = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).y / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, ROWS - 1);

			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
					//Если кликнуть закрытие окна
				case sf::Event::Closed:
				{
					//оно закроется..
					window.close();

					break;
				}
				case sf::Event::KeyReleased:
				{
					switch (event.key.code)
					{
					case sf::Keyboard::Enter:
					{
						//Перезапуск игры при нажатии "Enter"
						field.restart();
					}
					}

					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					switch (event.mouseButton.button)
					{
						//если нажата левая кнопка мыши
					case sf::Mouse::Left:
					{
						//то мы открываем клеточку
						field.open_cell(mouse_cell_x, mouse_cell_y);

						break;
					}
					//если нажата правая кнопка мыши
					case sf::Mouse::Right:
					{
						//то мы установим флаг на клетку
						field.flag_cell(mouse_cell_x, mouse_cell_y);
					}
					}
				}
				}
			}

			//Реакция смайлика на разные действия
			if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Left) || 1 == sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				//Открытие рта когда мы нажали на клетку
				Smile_state = 1;

				field.set_mouse_state(2, mouse_cell_x, mouse_cell_y);
			}
			else
			{
				//Закрытие рта когда нажатие закончилось
				Smile_state = 0;

				field.set_mouse_state(1, mouse_cell_x, mouse_cell_y);
			}

			//Если мы проиграли
			if (-1 == field.get_game_over())
			{
				//Смайлик меняет эмоцию
				Smile_state = 2;
			}
			//Если мы выйграли
			else if (1 == field.get_game_over())
			{
				//Смайлик снова меняет эмоцию
				Smile_state = 3;
			}

			if (FRAME_DURATION > lag)
			{
				//Очищаем окно
				window.clear();

				//прорисовываем игровое поле
				field.draw(window);

				//Информация о статусе игры проигрыш/победа
				if (1 == field.effect_over())
				{
					if (1 == field.get_game_over())
					{
						draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 8 * FONT_WIDTH))), static_cast<unsigned short>(round(0.5f * (CELL_SIZE * ROWS - FONT_HEIGHT))), "POBEDA!", window);
					}
					else
					{
						draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 4 * FONT_WIDTH))), static_cast<unsigned short>(round(0.5f * (CELL_SIZE * ROWS - 2 * FONT_HEIGHT))), "Proigrali...", window);
					}
				}

				//Счетчик мин
				draw_text(0, 0, CELL_SIZE * ROWS, "Mines:" + std::to_string(MINES - field.get_flags()), window);

				//Выбор смайлика из заготовки
				Smile.setTextureRect(sf::IntRect(Smile_state * Smile_texture.getSize().y, 0, Smile_texture.getSize().y, Smile_texture.getSize().y));

				//рисуем смайлик.
				window.draw(Smile);
				window.display();
			}
		}
	}
}