#pragma once
#include "SFML/Graphics.hpp"
class Floor
{
private:
	//Initialise the background texture and sprite
	sf::Texture floorTexture;
	
public:
	Floor();
	~Floor();
	void floorInit();
	sf::Sprite floor;
};

