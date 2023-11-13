#include "Font.h"


Font::Font(std::string t, int setTX, int setTY)
{
	setTextX = setTX;
	setTextY = setTY;

	text = t;
	// initialise font
	montserrat.loadFromFile("Assets/Montserrat-Regular.ttf");
	playerScoreText.setFont(montserrat);
	playerScoreText.setOutlineColor(sf::Color::Black);
	playerScoreText.setOutlineThickness(1.f);
	playerScoreText.setPosition(setTextX, setTextY);
}

Font::~Font()
{
}

void Font::Update(float dt)
{
	playerScoreText.setString(text);
}

// Rounds a float to two decimal places and turns it into a string
std::string Font::Stringify(float value)
{
	std::stringstream sStream;
	sStream << std::fixed << std::setprecision(2) << value;
	std::string t;
	sStream >> t;
	return t;
}

// Rounds turns int into a string
std::string Font::pStoString(int value)
{
	std::stringstream sStream;
	sStream << std::fixed << std::setprecision(2) << value;
	std::string t;
	sStream >> t;
	return t;
}

void Font::Render(sf::RenderWindow* window)
{
	window->draw(playerScoreText);
}
