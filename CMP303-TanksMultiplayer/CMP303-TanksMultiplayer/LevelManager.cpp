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
	p2p = new P2P(&tank);

	p2p->tcpListeningCheck();
	p2p->tcpStatusCheck();

	p2p->udpBindServer();
	p2p->udpBindClient();

	// initialising players tank
	//tank.push_back(new Tank("green", playerOneRotation));
	//tank[0]->setPosition(40, windowHeight / 2);
}

LevelManager::~LevelManager()
{
}

void LevelManager::Update(float dt)
{
	p2p->peerToPeerArchitecture(); // checking if host already exists. If not, the application is the host. if it is, the application will be a client
	
	// Class update functions
	if (tank.size() != 0) 
	{
		for (auto& t : tank)
		{
			t->Update(dt);
			t->handleInput(dt);
		}

	}
	//if (tank[0] && !playerOne)
	//{
	//	tank[0]->setPosition(40, windowHeight / 2);
	//	//tank[0]->setRotation(playerOneRotation);
	//	playerOne = true;
	//}
	//else if (tank[1] && !playerTwo)
	//{
	//	tank[1]->setPosition(windowWidth - 80, windowHeight / 2);
	//	playerTwo = true;
	//}
	//playerScore->Update(dt);
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
