#pragma once
#include <SFML\Graphics.hpp>
#include <sstream>
#include <iomanip>

class Font
{
private:
	int setTextX;
	int setTextY;

	//Initialise font and text
	sf::Font montserrat;
	sf::Text debugText;
public:
	Font(int setTX, int setTY);
	~Font();

	// Functions
	void Update(float dt);
	std::string Stringify(float value); // float to string. rounded up to 2 decimals
	std::string pStoString(int value); // int to string
	void Render(sf::RenderWindow* window);

};

