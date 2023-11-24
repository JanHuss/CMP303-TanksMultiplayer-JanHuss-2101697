#include <sstream>
#include <iomanip>
#include "LevelManager.h"
#include "Framework/Input.h"

int main() {
	//variables
	int windowWidth = 640;
	int windowHeight = 480;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "CMP303 - Tanks Multiplayer - Jan Huss - 2101697");
	window.setFramerateLimit(60);	//Request 60 frames per second

	Input* input;
	input = new Input;
	LevelManager level(input, windowWidth, windowHeight);

	// setting render mode
	//tank[3].SetRenderMode(Tank::RenderMode::REAL_AND_PREDICTED);

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
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) 
			{
				level.tanks[0]->handleInput(dt, *level.p2p);
				//level.p2p->sendTCPPacketServer(level.tanks[0]->movementPacket, level.p2p->client[0]);
				if (event.key.code == sf::Keyboard::Key::Escape)
				{
					// when the while loop ends send over the packet information to a client to let them know they are now the host
					sf::Packet tcpPacket;
					tcpPacket << "-HostHasLeft-";
					level.p2p->sendTCPPacketServer(tcpPacket, level.p2p->client[1]);
					window.close();
				}
			
				if (event.key.code == sf::Keyboard::Key::R) 
				{
					nextPrint = startTime;
					std::cout << "\n\n--------RESET--------\n\n" << std::endl;
				}
			}
		}
		level.Update(dt);

		//Render the scene
		window.clear();

		level.Render(&window);

		//window.draw(debugText);
		window.display();
	}

	return 0;
}