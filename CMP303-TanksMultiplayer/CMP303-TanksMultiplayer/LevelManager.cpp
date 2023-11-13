#include "LevelManager.h"

LevelManager::LevelManager(int windowW, int windowH)
{
	// initialisation
	windowWidth = windowW;
	windowHeight = windowH;
	playerTextX = windowW / 2;
	playerTextY = windowH / 2;
	playerScoreDisplay = "";

	tank = new Tank("", playerTextX, playerTextY, playerOneRotation);
	playerScore = new Font(playerScoreDisplay, playerTextX, playerTextY);
	floor = new Floor;
	floor->floorInit();

	// Networking Peer to Peer
	p2p = new P2P;
	p2p->tcpListeningCheck();
	p2p->tcpStatusCheck();

}

LevelManager::~LevelManager()
{
}

void LevelManager::Update(float dt)
{
	assignPlayer();
	p2p->socketSelection();

	// Class update functions
	tank->Update(dt);
	tank->handleInput(dt);
	playerScore->Update(dt);
}

void LevelManager::assignPlayer()
{
	// Networking
	if (p2p->getIsHost() && !playerOne) // if there is no host create host and player 1
	{
		p2p->checkIsHost(); // checking if host already exists. If not, the application is the host. if it is, the application will be a client
		// create player one tank
		tank = new Tank("green", playerTextX, playerTextY, playerOneRotation);
		playerScore = new Font("P1(Green): ", 20, 20);
		//tank.push_back(new Tank("green", playerTextX, playerTextY, playerOneRotation)); // player 1
		tank->setPosition(40, windowHeight / 2); // green tank (Player 1)
		playerOne = true;
	}
	else if (!p2p->getIsHost() && !playerTwo)
	{
		tank = new Tank("blue", playerTextX, playerTextY, playerTwoRotation);
		playerScore = new Font("P2(Blue): ", windowWidth - 80, 20);
		//tank.push_back(new Tank("blue", playerTextX, playerTextY, playerTwoRotation)); // player 2
		tank->setPosition(windowWidth - 40, windowHeight / 2); // blue tank (Player 2)
		playerTwo = true;
	}
	//else if (!p2p->getIsHost() && playerTwo && !playerThree)
	//{
	//	tank = new Tank("Red", playerTextX, playerTextY, playerTwoRotation);
	//	//tank.push_back(new Tank("Red", playerTextX, playerTextY, playerTwoRotation)); // player 2
	//	tank->setPosition(windowWidth / 2, 40); // red tank (Player 3)
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
std::string pStoString(int value)
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
	tank->Render(window);
	playerScore->Render(window);
}
