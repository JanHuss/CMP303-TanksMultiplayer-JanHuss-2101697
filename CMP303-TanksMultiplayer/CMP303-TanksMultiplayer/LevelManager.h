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
#include "Framework/Input.h"

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

	std::string playerScoreDisplay;

public:
	LevelManager(Input* in, int windowW, int windowH);
	~LevelManager();

	void Update(float dt);
	std::string pStoString(int value);
	void Render(sf::RenderWindow* window);
	// pointers
	Input* input;
	std::vector<Tank*> tanks; // create a tank object which will be assigned to a player
	Floor* floor; //Initialise the background texture and sprite
	//std::vector <Font*> playerScore; // Initialise font
	P2P* p2p; //Initialise Peer to Peer Network

};

