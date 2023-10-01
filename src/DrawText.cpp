#include <SFML/Graphics.hpp>

#include "DrawText.hpp"

void draw_text(bool i_black, unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window)
{
	//We're gonna align the text ot the left top
	short character_x = i_x;
	short character_y = i_y;

	unsigned char character_width;

	sf::Sprite character_sprite;

	sf::Texture font_texture;
	font_texture.loadFromFile("Resources/Images/Font.png");

	//Подсчет размеров символов в ширину
	//а 96 потому что их всего 96
	character_width = font_texture.getSize().x / 96;

	character_sprite.setTexture(font_texture);

	//Если мы хотим менять цвет нашего текста
	if (1 == i_black)
	{
		character_sprite.setColor(sf::Color(0, 0, 0));
	}

	for (const char a : i_text)
	{
		if ('\n' == a)
		{
			//После каждой строчки меняем координату у сбрасываем х
			character_x = i_x;
			character_y += font_texture.getSize().y;

			continue;
		}

		//Меняем положение след символа
		character_sprite.setPosition(character_x, character_y);
		//Выбор символа из текстури шрифта
		character_sprite.setTextureRect(sf::IntRect(character_width * (a - 32), 0, character_width, font_texture.getSize().y));

		//повышаем х координату
		character_x += character_width;

		//и рисуем символ
		i_window.draw(character_sprite);
	}
}