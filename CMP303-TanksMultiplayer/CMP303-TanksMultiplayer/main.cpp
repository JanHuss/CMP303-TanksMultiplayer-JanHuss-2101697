#include <SFML\Graphics.hpp>
#include <SFML/Network.hpp>
#include <sstream>
#include <iomanip>
#include "Tank.h"
#include "TankMessage.h"
#include "Framework/Input.h"

//Rounds a float to two decimal places and turns it into a string
std::string Stringify( float value ) {
	std::stringstream sStream;
	sStream << std::fixed << std::setprecision( 2 ) << value;
	std::string t;
	sStream >> t;
	return t;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(640, 480), "CMP303 - Tanks Multiplayer - Jan Huss - 2101697");
	window.setFramerateLimit(60);	//Request 60 frames per second
	
	//Create an Input opject
	Input* input = new Input;

	//Create two tanks (Can also accept "black" and "red")
	Tank tanks[4]{ Tank("green", input), Tank("blue", input), Tank("red", input), Tank ("black", input)};

	tanks[0].setPosition(64, 256); // green tank (Player 1)

	tanks[1].setPosition(416, 128); // blue tank (Player 2)

	tanks[2].setPosition(128, 64); // red tank (Player 3)

	tanks[3].setPosition(20, 20); // black tank (Player 4)

	tanks[1].SetRenderMode(Tank::RenderMode::REAL_AND_PREDICTED);

	//Initialise the background texture and sprite
	sf::Texture floorTexture;
	sf::Sprite floor;
	floorTexture.loadFromFile("Assets/tileSand1.png");
	floorTexture.setRepeated(true);
	floor.setTexture(floorTexture);
	floor.setTextureRect(sf::IntRect(0, 0, 640, 480));

	//Initialise font and text
	sf::Font montserrat;
	sf::Text debugText;
	montserrat.loadFromFile("Assets/Montserrat-Regular.ttf");
	debugText.setFont(montserrat);
	debugText.setOutlineColor(sf::Color::Black);
	debugText.setOutlineThickness(1.f);

	//Clock for timing the 'dt' value
	sf::Clock clock;
	float sendRate	= 0.5f;
	float latency	= 0.3f;
	float gameSpeed = 1.0f;
	float startTime = sendRate * 3.0f;

	// Networking variables
	sf::SocketSelector socketSelector;
	// setting up the TCP listener
	bool isHost = true; // bool to determine the host. if a host already exists then set to false and stop programme from listening for clients
	sf::TcpListener tcpListener;
	// setting up TCP socket
	sf::TcpSocket tcpSocket;

	if (tcpListener.listen(53000) != sf::Socket::Done)
	{
		printf("Failed to bind server to port\n");
		isHost = false;
	}
	else
	{
		socketSelector.add(tcpListener);
	}

	sf::Socket::Status tcpSocketStatus = tcpSocket.connect("localhost", 53000/*,timeOut*/);

	if (tcpSocketStatus != sf::Socket::Done)
	{
		printf("Error binding TCP socket\n"); // error message if TCP socket fails to bind
		//tcpSocket.send(packet);
		//tcpSocket.receive(packet);
	}
	
	printf("Socket is binding\n");
		
	

	//When are we next printing the predicted position (so we don't spam the console)
	float nextPrint = startTime;

	//Create a network simulator with that "sends" a message every 0.5 seconds and has a latency of 0.1 seconds
	//NetworkSimulator netSimulator(sendRate, latency);
	//netSimulator.m_MyID = 0;	//On the network, we are Tank 0
	
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
					tanks[0].Reset(); tanks[1].Reset();
					//netSimulator.Reset();
					nextPrint = startTime;
					printf( "\n\n--------RESET--------\n\n" );
				}
			}
		}
		// Networking
		// setting up packet
		sf::Packet packet;
		
		
		if (isHost)
		{
			if (socketSelector.wait(sf::milliseconds(1)))
			{
				if (socketSelector.isReady(tcpListener))
				{
					printf("Is Host\n");
					sf::TcpSocket client;
					if (tcpListener.accept(client) != sf::Socket::Done)
					{
						printf("Failed to connect to client\n");
					}
					printf("A client connected\n");
				}
			}
		}
		
		//sf::Time timeOut;
		socketSelector.add(tcpSocket);
		tcpSocket.setBlocking(false);
		if (socketSelector.wait(sf::milliseconds(1)))
		{
			if (socketSelector.isReady(tcpSocket))
			{
				
			}
		}

		//Render the scene
		window.clear();
		window.draw(floor);
		for (auto& tank : tanks) {
			tank.Render(&window);
		}
		window.draw(debugText);
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