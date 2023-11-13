#include <SFML\Graphics.hpp>
#include <sstream>
#include <iomanip>
#include "Floor.h"
#include "LevelManager.h"

int main() {
	//variables
	int windowWidth = 640;
	int windowHeight = 480;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "CMP303 - Tanks Multiplayer - Jan Huss - 2101697");
	window.setFramerateLimit(60);	//Request 60 frames per second

	LevelManager level;

	// setting render mode
	//tank[3].SetRenderMode(Tank::RenderMode::REAL_AND_PREDICTED);

	//Initialise the background texture and sprite
	Floor* floor = new Floor;
	floor->floorInit();

	//Clock for timing the 'dt' value
	sf::Clock clock;
	float sendRate	= 0.5f;
	float latency	= 0.3f;
	float gameSpeed = 1.0f;
	float startTime = sendRate * 3.0f;

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
		
		//Render the scene
		window.clear();
		window.draw(floor->floor);
		
		level.Render(&window);

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