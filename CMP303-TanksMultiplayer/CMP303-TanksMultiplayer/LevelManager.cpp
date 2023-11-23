#include "LevelManager.h"

LevelManager::LevelManager(Input* in, int windowW, int windowH)
{
	// initialisation
	input = in;
	windowWidth = windowW;
	windowHeight = windowH;
	playerTextX = windowW / 2;
	playerTextY = windowH / 2;
	playerScoreDisplay = "";

	//playerScore = new Font(playerScoreDisplay, playerTextX, playerTextY);
	floor = new Floor;
	floor->floorInit();

	// initialising players tank
	tanks.push_back(new Tank("green", playerOneRotation, input));
	tanks[0]->setPosition(40, windowHeight / 2);

	// Networking Peer to Peer
	p2p = new P2P(tanks, input, windowWidth, windowHeight);
	

	p2p->tcpListeningCheck();
	p2p->tcpStatusCheck();

	p2p->udpBindServer();
	p2p->udpBindClient();

}

LevelManager::~LevelManager()
{
}

void LevelManager::Update(float dt)
{
	p2p->HostClientArchitecture(); // checking if host already exists. If not, the application is the host. if it is, the application will be a client
	
	// Class update functions
	
	tanks[0]->Update(dt);
	//tanks[0]->handleInput(dt, *p2p);
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
	if (tanks.size() != 0)
	{
		for (auto& t : tanks) 
		{
			t->Render(window);
		}
	}
	//playerScore->Render(window);
}
