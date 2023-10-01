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
	//������������� ���� �� ������� ������
	unsigned lag = 0;

	//������� ����� 4 ������:
	//0) ��������
	//1) ��������� ������
	//2) ����� �� ��������� :(
	//3) ����� �� ��������! :)
	unsigned char Smile_state = 0;

	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	sf::Event event;

	//������ ��� SFML
	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * COLUMNS * SCREEN_RESIZE, SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * COLUMNS)), "SAPER!", sf::Style::Close);
	//������ ������ ����
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * COLUMNS, FONT_HEIGHT + CELL_SIZE * ROWS)));

	//�������� ��������.
	sf::Sprite Smile;

	//��������� ��������.
	sf::Texture Smile_texture;
	//����������� ���������
	Smile_texture.loadFromFile("Resources/Images/Smile.png");

	//�������� �������� � �������� ����
	Smile.setPosition(static_cast<float>(CELL_SIZE * COLUMNS - Smile_texture.getSize().y)/2, CELL_SIZE * ROWS);
	Smile.setTexture(Smile_texture);

	//����
	Field field;

	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		//��� ������������� ��������
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

		lag += delta_time;

		previous_time += std::chrono::microseconds(delta_time);

		//���� �������� ��������� ����������� ���������� ������������ �����
		while (FRAME_DURATION <= lag)
		{
			//����� �� ����� �������� ���������� ������, ������� ��������� ��� �������� �� ����� �����
			unsigned char mouse_cell_x = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).x / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, COLUMNS - 1);
			unsigned char mouse_cell_y = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).y / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, ROWS - 1);

			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
					//���� �������� �������� ����
				case sf::Event::Closed:
				{
					//��� ���������..
					window.close();

					break;
				}
				case sf::Event::KeyReleased:
				{
					switch (event.key.code)
					{
					case sf::Keyboard::Enter:
					{
						//���������� ���� ��� ������� "Enter"
						field.restart();
					}
					}

					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					switch (event.mouseButton.button)
					{
						//���� ������ ����� ������ ����
					case sf::Mouse::Left:
					{
						//�� �� ��������� ��������
						field.open_cell(mouse_cell_x, mouse_cell_y);

						break;
					}
					//���� ������ ������ ������ ����
					case sf::Mouse::Right:
					{
						//�� �� ��������� ���� �� ������
						field.flag_cell(mouse_cell_x, mouse_cell_y);
					}
					}
				}
				}
			}

			//������� �������� �� ������ ��������
			if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Left) || 1 == sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				//�������� ��� ����� �� ������ �� ������
				Smile_state = 1;

				field.set_mouse_state(2, mouse_cell_x, mouse_cell_y);
			}
			else
			{
				//�������� ��� ����� ������� �����������
				Smile_state = 0;

				field.set_mouse_state(1, mouse_cell_x, mouse_cell_y);
			}

			//���� �� ���������
			if (-1 == field.get_game_over())
			{
				//������� ������ ������
				Smile_state = 2;
			}
			//���� �� ��������
			else if (1 == field.get_game_over())
			{
				//������� ����� ������ ������
				Smile_state = 3;
			}

			if (FRAME_DURATION > lag)
			{
				//������� ����
				window.clear();

				//������������� ������� ����
				field.draw(window);

				//���������� � ������� ���� ��������/������
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

				//������� ���
				draw_text(0, 0, CELL_SIZE * ROWS, "Mines:" + std::to_string(MINES - field.get_flags()), window);

				//����� �������� �� ���������
				Smile.setTextureRect(sf::IntRect(Smile_state * Smile_texture.getSize().y, 0, Smile_texture.getSize().y, Smile_texture.getSize().y));

				//������ �������.
				window.draw(Smile);
				window.display();
			}
		}
	}
}