#include <SFML\Graphics.hpp>
//#include <SFML/Network.hpp>
#include <sstream>
#include <iomanip>
//#include "Tank.h"
//#include "TankMessage.h"
//#include <vector>
#include "Floor.h"
//#include "P2P.h"

#include "LevelManager.h"


//Rounds a float to two decimal places and turns it into a string
//std::string Stringify( float value ) 
//{
//	std::stringstream sStream;
//	sStream << std::fixed << std::setprecision( 2 ) << value;
//	std::string t;
//	sStream >> t;
//	return t;
//}
//
////Rounds turns int into a string
//std::string pStoString(int value)
//{
//	std::stringstream sStream;
//	sStream << std::fixed << std::setprecision(2) << value;
//	std::string t;
//	sStream >> t;
//	return t;
//}

int main() {
	//variables
	int windowWidth = 640;
	int windowHeight = 480;
	//int playerTextX = windowWidth / 2;
	//int playerTextY = windowHeight / 2;
	//int playerOneRotation = 270;
	//int playerTwoRotation = 90;
	//int playerThreeRotation = 0;
	//
	//bool playerOne = false;
	//bool playerTwo = false;
	//bool playerThree = false;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "CMP303 - Tanks Multiplayer - Jan Huss - 2101697");
	window.setFramerateLimit(60);	//Request 60 frames per second

	LevelManager level;

	//Create four tanks using a vector pointer of tanks
	//std::vector<Tank*> tank;
	//Tank* tank = new Tank("green", playerTextX, playerTextY, playerOneRotation);
	
	// Create a vector of tanks
	//tank.push_back(new Tank("green", playerTextX, playerTextY, playerOneRotation)); // player 1
	//tank.push_back(Tank("blue", input)); // player 2
	//tank.push_back(Tank("red", input)); // player 3
	//tank.push_back(Tank("black", input)); // player 4
	
	// set tank positions
	//tank[0]->setPosition(40, windowHeight / 2); // green tank (Player 1)
	//tank[1].setPosition(windowWidth - 40, windowHeight / 2); // blue tank (Player 2)
	//tank[2].setPosition(windowWidth / 2, 40); // red tank (Player 3)
	//tank[3].setPosition(windowWidth / 2, windowHeight - 40); // black tank (Player 4)

	// setting render mode
	//tank[3].SetRenderMode(Tank::RenderMode::REAL_AND_PREDICTED);

	//Initialise the background texture and sprite
	Floor* floor = new Floor;
	floor->floorInit();

	//Initialise font and text
	//sf::Font montserrat;
	//sf::Text debugText;
	//montserrat.loadFromFile("Assets/Montserrat-Regular.ttf");
	//debugText.setFont(montserrat);
	//debugText.setOutlineColor(sf::Color::Black);
	//debugText.setOutlineThickness(1.f);

	//Initialise Network Peer to Peer
	//P2P* p2p = new P2P;

	//Clock for timing the 'dt' value
	sf::Clock clock;
	float sendRate	= 0.5f;
	float latency	= 0.3f;
	float gameSpeed = 1.0f;
	float startTime = sendRate * 3.0f;

	//p2p->tcpListeningCheck();
	//
	//p2p->tcpStatusCheck();	

	//When are we next printing the predicted position (so we don't spam the console)
	float nextPrint = startTime;
	
	while (window.isOpen()) {
		//Get the time since the last frame in milliseconds
		float dt = clock.restart().asSeconds() * gameSpeed;

		sf::Event event;
		while (window.pollEvent(event))	{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Key::Escape)
					window.close();
				if( event.key.code == sf::Keyboard::Key::R ) {
					//for (auto t : tank) // resetting tanks position in for loop
					//{
					//	t.Reset();
					//}
					
					//netSimulator.Reset();
					nextPrint = startTime;
					printf( "\n\n--------RESET--------\n\n" );
				}
			}
		}
		level.Update(dt);
		//// Networking
		//if (p2p->getIsHost() && !playerOne) // if there is no host create host and player 1
		//{
		//	p2p->checkIsHost(); // checking if host already exists. If not, the application is the host. if it is, the application will be a client
		//	// create player one tank
		//	tank = new Tank("green", playerTextX, playerTextY, playerOneRotation);
		//	//tank.push_back(new Tank("green", playerTextX, playerTextY, playerOneRotation)); // player 1
		//	tank->setPosition(40, windowHeight / 2); // green tank (Player 1)
		//	playerOne = true;
		//}
		//else if (!p2p->getIsHost() && !playerTwo)
		//{
		//	tank = new Tank("blue", playerTextX, playerTextY, playerTwoRotation);
		//	//tank.push_back(new Tank("blue", playerTextX, playerTextY, playerTwoRotation)); // player 2
		//	tank->setPosition(windowWidth - 40, windowHeight / 2); // blue tank (Player 2)
		//	playerTwo = true;
		//}
		//else if (!p2p->getIsHost() && playerTwo && !playerThree)
		//{
		//	tank = new Tank("Red", playerTextX, playerTextY, playerTwoRotation);
		//	//tank.push_back(new Tank("blue", playerTextX, playerTextY, playerTwoRotation)); // player 2
		//	tank->setPosition(windowWidth / 2, 40); // red tank (Player 3)
		//	playerThree = true;
		//}
		//p2p->socketSelection();		
		//
		//// Class update functions
		//tank->Update(dt);
		//tank->handleInput(dt);
		//Render the scene
		window.clear();
		window.draw(floor->floor);
		//for (int i = 0; i< tank.size();i++)
		//{
		level.Render(&window);
			//tank->Render(&window);
		//}
		//window.draw(debugText);
		window.display();		
	}

	// setting up UDP socket
	//sf::UdpSocket udpSocket;
	//if (udpSocket.bind(54000) != sf::Socket::Done)// error message if UDP socket fails to bind
	//{
	//	std::cout << "Error binding UDP socket";
	//udpSocket.send(packet, recipientAddress, recipientPort);
	//udpSocket.receive(packet, senderAddress, senderPort);
	//}

	return 0;
}