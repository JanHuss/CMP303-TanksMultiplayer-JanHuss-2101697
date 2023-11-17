#include "LevelManager.h"

LevelManager::LevelManager(int windowW, int windowH)
{
	// initialisation
	windowWidth = windowW;
	windowHeight = windowH;
	playerTextX = windowW / 2;
	playerTextY = windowH / 2;
	playerScoreDisplay = "";

	//playerScore = new Font(playerScoreDisplay, playerTextX, playerTextY);
	floor = new Floor;
	floor->floorInit();

	// Networking Peer to Peer
	p2p = new P2P(tank);
	p2p->tcpListeningCheck();
	p2p->tcpStatusCheck();
	p2p->udpBindServer();
	p2p->udpBindClient();
	tank.push_back(new Tank("green", playerOneRotation));
	tank[0]->setPosition(40, windowHeight / 2);
}

LevelManager::~LevelManager()
{
}

void LevelManager::Update(float dt)
{
	//p2p->recievePacket();
	//std::cout << "tank vector size: " + pStoString(tank.size()) << std::endl;
	//assignPlayer();
	//p2p->socketSelection();
	p2p->checkIfThereIsHost(); // checking if host already exists. If not, the application is the host. if it is, the application will be a client
	// Class update functions
	if (tank.size() != 0) 
	{
		for (auto& t : tank)
		{
			t->Update(dt);
			t->handleInput(dt);
		}

	}
	
	//playerScore->Update(dt);
}

void LevelManager::assignPlayer()
{
	// Networking
	//if (p2p->getIsHost() && !playerOne) // if there is no host create host and player 1
	//{
	//	p2p->checkIfThereIsHost(); // checking if host already exists. If not, the application is the host. if it is, the application will be a client
	//	// create player one tank
	//	tank.push_back(new Tank("green", playerOneRotation));
	//	//playerScore = new Font("P1(Green): ", 20, 20); // player 1
	//	tank[0]->setPosition(40, windowHeight / 2); // green tank (Player 1)
	//	playerOne = true;
	//	
	//}
	//else if (!p2p->getIsHost())
	//{
	//	// player 2
	//	tank.push_back(new Tank("blue", playerTwoRotation));
	//	playerScore = new Font("P2(Blue): ", windowWidth - 80, 20); // player 2
	//	tank[0]->setPosition(windowWidth - 40, windowHeight / 2); // blue tank (Player 2)
	//	// player 1
	//	tank.push_back(new Tank("green", playerOneRotation));
	//	playerScore = new Font("P1(Green): ", 20, 20); // player 1
	//	tank[1]->setPosition(40, windowHeight / 2); // green tank (Player 1)
	//	playerTwo = true;
	//}
	//else if (!p2p->getIsHost() && playerTwo && !playerThree)
	//{
	//	tank.push_back(new Tank("Red", playerTwoRotation)); // player 3
	//	tank[2]->setPosition(windowWidth / 2, 40); // red tank (Player 3)
	//	playerThree = true;
	//}
}

//Rounds a float to two decimal places and turns it into a string
std::string Stringify(float value)
{
	std::stringstream sStream;
	sStream << std::fixed << std::setprecision(2) << value;
	std::string t;
	sStream >> t;
	return t;
}

//Rounds turns int into a string
std::string LevelManager::pStoString(int value)
{
	std::stringstream sStream;
	sStream << std::fixed << std::setprecision(2) << value;
	std::string t;
	sStream >> t;
	return t;
}

void LevelManager::Render(sf::RenderWindow* window)
{
	floor->Render(window);
	if (tank.size() != 0)
	{
		for (auto& t : tank) 
		{
			t->Render(window);
		}
	}
	//playerScore->Render(window);
}
