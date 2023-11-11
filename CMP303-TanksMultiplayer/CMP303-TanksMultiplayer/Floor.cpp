#include "Floor.h"

Floor::Floor()
{
	floorTexture.loadFromFile("Assets/tileSand1.png");
}

Floor::~Floor()
{
}

void Floor::floorInit()
{
	floorTexture.setRepeated(true);
	floor.setTexture(floorTexture);
	floor.setTextureRect(sf::IntRect(0, 0, 640, 480));
}
