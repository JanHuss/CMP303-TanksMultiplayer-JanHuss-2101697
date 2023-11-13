#pragma once

#include <SFML\Graphics.hpp>
#include <SFML/Network.hpp>
#include <sstream>
#include <iomanip>
#include "Tank.h"
#include "TankMessage.h"
#include <vector>
#include "Floor.h"
#include "Font.h"
#include "P2P.h"

class LevelManager
{
private:
	//variables
	int windowWidth;
	int windowHeight;
	int playerTextX;
	int playerTextY;
	int playerOneRotation = 270;
	int playerTwoRotation = 90;
	int playerThreeRotation = 0;

	bool playerOne = false;
	bool playerTwo = false;
	bool playerThree = false;

public:
	LevelManager(int windowW, int windowH);
	~LevelManager();

	void Update(float dt);
	void assignPlayer();
	void Render(sf::RenderWindow* window);

	//Create four tanks using a vector pointer of tanks
	//std::vector<Tank*> tank;

	// pointers
	Tank* tank; // create a tank object which will be assigned to a player
	Floor* floor; //Initialise the background texture and sprite
	Font* font; // Initialise font
	P2P* p2p; //Initialise Peer to Peer Network
};

