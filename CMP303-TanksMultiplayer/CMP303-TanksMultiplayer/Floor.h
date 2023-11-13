#pragma once
#include "SFML/Graphics.hpp"
class Floor
{
private:
	//Initialise the background texture and sprite
	sf::Texture floorTexture;
	sf::Sprite floor;
	
public:
	Floor();
	~Floor();
	void floorInit();
	void Render(sf::RenderWindow* window);
};

