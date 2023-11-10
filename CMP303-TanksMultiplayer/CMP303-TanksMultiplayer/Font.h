#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include <iomanip>
class Font
{
private:
	//Initialise font and text
	sf::Font montserrat;
	//sf::Text debugText;
	sf::Text pScoreText;
	sf::RenderWindow* window;

public:
	Font(sf::RenderWindow* w);
	~Font();

	void update();
	void setFontPos(int x, int y);

	sf::Vector2f getFontPos();

	void Render(sf::RenderWindow* window);
};

