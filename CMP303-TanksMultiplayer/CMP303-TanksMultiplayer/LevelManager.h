#pragma once

#include <SFML\Graphics.hpp>
#include <SFML/Network.hpp>
#include <sstream>
#include <iomanip>
#include "Tank.h"
#include "TankMessage.h"
#include <vector>
#include "Floor.h"
#include "P2P.h"

class LevelManager
{
private:
	//variables
	int windowWidth = 640;
	int windowHeight = 480;
	int playerTextX = windowWidth / 2;
	int playerTextY = windowHeight / 2;
	int playerOneRotation = 270;
	int playerTwoRotation = 90;
	int playerThreeRotation = 0;

	bool playerOne = false;
	bool playerTwo = false;
	bool playerThree = false;

public:
	LevelManager();
	~LevelManager();

	void Update(float dt);
	void Render(sf::RenderWindow* window);

	//Create four tanks using a vector pointer of tanks
	//std::vector<Tank*> tank;
	Tank* tank;
	////Initialise the background texture and sprite
	Floor* floor;
	//Initialise Network Peer to Peer
	P2P* p2p;

};

