#include "Font.h"
#include <sstream>

Font::Font(sf::RenderWindow* w) : window(w)
{
	montserrat.loadFromFile("Assets/Montserrat-Regular.ttf");
	// set font
	pScoreText.setFont(montserrat);
	pScoreText.setOutlineColor(sf::Color::Black);
	pScoreText.setOutlineThickness(1.f);
	//pScoreText.setPosition(5, 5);
}

Font::~Font()
{

}

void Font::update() 
{
	pScoreText.setString("P1: " /*+ StringScore(0)*/);
}

void Font::setFontPos(int x, int y)
{
	pScoreText.setPosition(x, y);
}

sf::Vector2f Font::getFontPos()
{
	return pScoreText.getPosition();
}

void Font::Render(sf::RenderWindow* window)
{	
	window->draw(pScoreText);
}

std::string StringScore(int value) 
{
	std::stringstream sStream;
	sStream << std::fixed << std::setprecision(2) << value;
	std::string t;
	sStream >> t;
	return t;
}
