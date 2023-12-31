#include "P2P.h"

P2P::P2P(std::vector<Tank*>& t, Input* in, int winW, int winH) : tanks(t), input(in), windowWidth(winW), windowHeight(winH)
{
	hasJoined = "A client has joined";
	setIsHost(true);
	isPlayerTwo = true;
	isPlayerThree = false;
	isPlayerFour = false;
	maxPlayers = 4;

	goNorth = 180;
	goSouth = 0;
	goEast = 270;
	goWest = 90;

	locations.push_back(sf::Vector3f(40, windowHeight / 2, goEast));
	locations.push_back(sf::Vector3f(windowWidth - 80, windowHeight / 2, goWest));
	locations.push_back(sf::Vector3f(windowWidth / 2, 40, goSouth));
	locations.push_back(sf::Vector3f(windowWidth / 2, windowHeight - 80,goNorth));

	windowOpen = true;
}
P2P::~P2P()
{
	for (auto c : client)
	{
		if (c)
		{
			c = nullptr;
		}
	}
	for (auto t: tanks)
	{
		if(t)
		{
			t = nullptr;
		}
	}
	if (input)
	{
		input = nullptr;
	}
}

// --- Peer to Peer Architecture ---
void P2P::HostClientArchitecture()
{		
	if (socketSelector.wait(sf::milliseconds(1))) // Returns a boolean as soon as at least one socket has some data available to be received.
	{
		// --- Server Area ---
		serverSetup();	// SERVER section setup. The architecture became too convoluted to maintain in singular function.
						// This section maintains the SERVER/HOST architecture in which the application listens through the local host if there is a HOST/SERVER present.
						// If not, the application will make itself the HOST/SERVER and will listen for any CLIENTS to join them.

		// --- Client Area ---
		clientSetup();	// CLIENT section setup. The architecture became too convoluted to maintain in singular function.
						// This section maintains the CLIENT architecture in which the application listens for a HOST/SERVER they can join.
						// As this is a peer to peer networking architecture, the HOST/SERVER will join as a CLIENT themselves
	}
}
void P2P::serverSetup()
{
	// ------------------------------------------------ Server Area ------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------
	if (socketSelector.isReady(tcpListener)/* && isHost*/)	// This function must be used after a call to Wait, to know
														// which sockets are ready to receive data and will prevent 
														// blocking because there is data to be received.
														// Also, if host is true, the application turns to be a client
	{
		std::cout << "----------------------------------------------------------------------------\n";
		std::cout << "--------------------------------- Server -----------------------------------\n\n";
		// TCP Section -----------------------------------------------------------------------------------------------
		sf::TcpSocket* tempTCPSocket = new sf::TcpSocket; // create a new temporary TCP socket pointer

		if (tcpListener.accept(*tempTCPSocket) != sf::Socket::Done) //If the socket is in blocking mode, this function will not return until a connection is actually received.
		{
			std::cout << "!!! Error - SERVER ---> TCP ---> Failed to connect to client !!!\n";	// If not accepting the temporary TCP socket pointer,
		}																							// print error indicating that the TCP socket pointer failed to connect to CLIENT

		tempTCPSocket->setBlocking(false); // Set blocking to false. Calls will always return immediately, using the return code to signal whether there was data available or not.
		socketSelector.add(*tempTCPSocket); // Add the temporary TCP socket pointer to the socket selector, which is a multiplexer that allows to read from multiple sockets. 
		int playerID = client.size() + 1;
		while (true)
		{
			for (auto& c : client)
			{
				if (playerID == c->playerID)
				{
					playerID++;
					continue;
				}
			}
			break;
		}
		Client* playerClient = new Client(tempTCPSocket, playerID); // Create a CLIENT struct pointer and assign it a new CLIENT struct, passing through the temp TCP socket essentially assigning a socket to the client

		client.push_back(playerClient); // Add the newly created CLIENT struct with the assigned temp TCP socket to the client struct vector. This means that a player has connected with the HOST/SERVER
										// and will be used to receive information from the CLIENTS but also to send data back out to the CLIENTS
		std::cout << "- SERVER ---> TCP ---> A new CLIENT has connected\n\n"; // Message that a CLIENT has connected
		std::cout << "### Number of CLIENTS connected: " << client.size() << " ###\n\n"; // Message to show how many CLIENTS have joined the network

		// --- Packet SERVER side TCP: SENDING ---
		sf::Packet packet; // Create a packet
		std::string clientEntersRoom = "Welcome to SERVER"; // Create a test string with a message to put into packet
		packet << clientEntersRoom << playerID << locations[client.size()-1].x << locations[client.size()-1].y << locations[client.size()-1].z; // Pack the test string into the packet
		sendTCPPacketServer(packet, playerClient); // send the packet along with the playerClient pointer containing the temp TCP socket to the CLIENT
		
		for (auto& c : client)
		{
			if (playerID != c->playerID)
			{
				sf::Packet playerDataTwo;
				playerDataTwo << "playerJoinedServer" << c->playerPos.x << c->playerPos.y << c->playerID;
				sendTCPPacketServer(playerDataTwo, playerClient);
			}
		}
	}
	else	// if the socket selector is not ready, block and iterate through the client vector and assign the ID to each client
			// the server will then utilise this to determine which player is which and will relay this over to the other clients
	{
		
		
		for (auto& c : client) // Iterate through the client struct vector. Every c in client resembles a CLIENT/Player
		{
			// --- Packet SERVER side TCP: RECEIVING ---
			if (socketSelector.isReady(*c->tcpID))	// This function must be used after a call to Wait, to know which sockets are ready to receive data. 
			{										//If a socket is ready, a call to receive will never block because we know that there is data available to read.
				sf::Packet packet; // Create a new packet for the server to receive information from a CLIENT
				sf::Socket::Status status = c->tcpID->receive(packet); // Create a new socket status set to give a status on whether the CLIENT packet information has been received
				
				if (status == sf::Socket::Disconnected)
				{
					std::cout << "Player disconnected from SERVER\n";
					// remove from socket
					socketSelector.remove(*c->tcpID);
					// disconnect client
					c->tcpID->disconnect();

					auto i = std::find(client.begin(), client.end(), c);
					auto t = std::find(tanks.begin(), tanks.end(), getTank(c->playerID));
					if (i != client.end())
					{
						client.erase(i);
					}
					if (t != tanks.end())
					{
						tanks.erase(t);
					}
					continue;
				}
				// Initially Binding SERVER UDP
				std::string clientEntersRoom; // Create a string for the receiving packet to unload on
				packet >> clientEntersRoom; // Pass packet information to string
				std::cout << "Client enters room TCP" << clientEntersRoom << "\n"; // Output received data which should be PlayerJoined
				
				if (clientEntersRoom._Equal("PlayerJoined")) // Check if the string is equal to the string of the socket that binds the UDP SERVER function
				{
					float tankX;
					float tankY;

					unsigned short port; // create an unsigned short variable for the received UDP packet to pass data to
					packet >> port >> tankX >> tankY; // pass data from received packet to the unsigned short variable
					c->playerPos.x = tankX;
					c->playerPos.y = tankY;

					for (auto& cTwo : client)
					{
						if (c->playerID != cTwo->playerID)
						{
							sf::Packet playerData;
							playerData << "playerJoinedServer" << c->playerPos.x << c->playerPos.y << c->playerID;

							sendTCPPacketServer(playerData, cTwo);
						}
					}

					c->port = port; // assign the unsigned short variable to the current CLIENT'S port variable
					std::cout << "- SERVER ---> UDP ---> Client port: " << port << "\n"; // Output the current CLIENT'S port number
					sf::Packet udpPacket; // Create a new packet that can be sent via UDP
					udpPacket << "- SERVER ---> UDP ---> sending test packet "; // Pass in string to send a test message

					//sendUDPPacketServer(udpPacket, c); // Pass back information via UDP from the information received by the other CLIENTS???
				}				
			}
			// UDP Section -------------------------------------------------------------------------------------------
				// --- Packet SERVER side UDP: RECEIVING --- 
			std::string clientEntersRoom; // string for the test string PlayerJoined being sent
			std::string clientData; // string that contains the data the CLIENT has sent
			sf::Packet uDPPacket; // UDP Packet

			uDPPacket = receiveUDPPacketServer();
			uDPPacket >> clientData;

			if (clientData._Equal("PlayerMovement"))
 			{
				float tankX;
				float tankY;
				sf::Vector2f speed;
				int tankID;
				float rotation;
				uDPPacket >> tankX >> tankY >> rotation >> speed.x >> speed.y >> tankID; // pass data from received packet to the unsigned short variable
				c->playerPos.x = tankX;
				c->playerPos.y = tankY;

				//std::cout << "got id; " << tankID << " - " << c->playerID << std::endl;

				for (auto& cTwo : client)
				{
					if (c->playerID != cTwo->playerID)
					{
						sf::Packet playerData;
						
						playerData << "PlayerMovement" << c->playerPos.x << c->playerPos.y << rotation << speed.x << speed.y << tankID;
						
						sendUDPPacketServer(playerData, cTwo);
					}
				}
			}
		}
	}
}
void P2P::clientSetup()
{
	// ------------------------------------------------ Client Area ------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------
	if (socketSelector.isReady(tcpSocket)) // This function must be used after a call to Wait, to know which TCP sockets are ready to receive data. 
	{
		if (!getIsHost()) // section header for CLIENT
		{
			std::cout << "----------------------------------------------------------------------------\n";
			std::cout << "--------------------------------- Client -----------------------------------\n\n";
		}

		// --- Packet CLIENT side TCP: SENDING ---
		sf::Packet sendTCPPacket; // create a Packet for sending over to the Server
		sendTCPPacket << "--- CLIENT TO SERVER ---> PACKET ---> TCP ---> Hi, I'm A CLIENT. I hope you don't mind me joining =)\n"; // put a test string into packet which will let the SERVER know that a CLIENT has joined
		sendPacketClient(sendTCPPacket); // send packet to SERVER via TCP

		// --- Packet CLIENT side TCP: RECEIVING --- 
		sf::Packet receiveTCPPacket = receiveTCPPacketClient(tcpSocket); // create a packet to receive via TCP and set the receive via TCP function for the CLIENT with the assigned TCP socket as an argument to that packet
		std::string clientReceivesTCPPacket; // string to unload the TCP packet information on to

		receiveTCPPacket >> clientReceivesTCPPacket; // pass data from TCP packet received to the string
		std::cout << "Client receives TCP packet" << clientReceivesTCPPacket << "\n"; // output data received from the SERVER
		if (clientReceivesTCPPacket._Equal("Welcome to SERVER"))
		{
			int pID;
			sf::Vector3f tankLocs;
			receiveTCPPacket >> pID >> tankLocs.x >> tankLocs.y >> tankLocs.z;
			tanks[0]->setTankID(pID);
			tanks[0]->setPosition(tankLocs.x, tankLocs.y);
			tanks[0]->setRotation(tankLocs.z);
			std::cout << "your id is: " << pID << "\n";
		}
		if (clientReceivesTCPPacket._Equal("-HostHasLeft-"))
		{
			windowOpen = false;
		}
		if (clientReceivesTCPPacket._Equal("playerJoinedServer"))
		{
			int playerID;
			float tankX;
			float tankY;
			sf::Vector2f speed;
			receiveTCPPacket >> tankX >> tankY >> playerID;
			Tank* tank = new Tank("blue", 90, input);
			tank->setPosition(tankX, tankY);
			tank->setTankID(playerID);
			
			tanks.push_back(tank);

			for (const auto& t : tanks) 
			{
				std::cout << "ID; " << t->getTankID() << "\n";
			}

			std::cout << "new player id: " << playerID << "\n";
		}
	}
	if (socketSelector.isReady(udpSocketClient)) // This function must be used after a call to Wait, to know if UDP sockets are ready to receive data.
	{
		// --- Packet CLIENT side UDP: RECEIVING ---
		std::string clientData; // string to move the CLIENT information received via UDP 
		sf::Packet udpPacket = receiveUDPPacketClient(); // move the received UDP packet information onto the string
		udpPacket >> clientData;
		if (clientData.empty())
		{
			return;
		}
		if (clientData._Equal("PlayerMovement"))
		{
			float tankX;
			float tankY;
			float rotation;
			int tankID;
			sf::Vector2f speed;
			udpPacket >> tankX >> tankY >> rotation >> speed.x >> speed.y >> tankID;
			Tank* t = getTank(tankID);
			t->setPosition(tankX, tankY);
			t->setRotation(rotation);
			t->predictions.push_back(new Prediction(currentTime, sf::Vector2f(tankX, tankY), speed));

			if (t->predictions.size() > 3)
			{
				t->predictions.pop_front();
			}
			Prediction* p = prediction(t);
			t->setGhostPosition(p->tankPos);
		}
		//std::cout << "Client receives UDP packet: " << clientData << "\n"; // output the received data
	}
	
}

// --- TCP Setup ---
void P2P::tcpListeningCheck()// SERVER looking/listening for available socket
{
	if (tcpListener.listen(53000) != sf::Socket::Done)
	{
		std::cout << "!!! Error - SERVER --- TCP ---> Failed to bind server to TCP port !!!" << "\n";
		setIsHost(false);
	}
	
	socketSelector.add(tcpListener);	
}
void P2P::tcpStatusCheck() // CLIENT - attempts to connect to tcp socket
{
	const sf::Socket::Status tcpSocketStatus = tcpSocket.connect("localhost", 53000);
	tcpListener.setBlocking(false);
	socketSelector.add(tcpSocket);
	if (tcpSocketStatus != sf::Socket::Done)
	{
		std::cout << "Error -- CLIENT --- TCP ---> Cannot bind to TCP socket\n"; // error message if TCP socket fails to bind
	}
	
	std::cout << "-- CLIENT --- TCP ---> TCP socket is binding" << tcpSocketStatus << "\n";
}

// --- TCP Sending/Receiving ---
void P2P::sendPacketClient(sf::Packet p)
{
	const sf::Socket::Status status = tcpSocket.send(p); // assign the packet information that is being sent to a status variable
	if (status != sf::Socket::Done) // if the data sent is not 0 then the data will only be partially have been sent or not at all
	{
		std::cout << "!!! Error --- PACKET ---> TCP ---> Cannot send data !!!\n"; // output error if the status is not 0
	}
}
void P2P::sendTCPPacketServer(sf::Packet p, const Client* pCP)	// Send a packet from the SERVER to the CLIENT with the dedicated ID which is being passed through as a pCP argument
{
	const sf::Socket::Status status = pCP->tcpID->send(p);	// Create a status variable and assign the CLIENT ID send information to it
	if (status != sf::Socket::Done)		// Utilising the CLIENT ID send information, check if the socket hasn't sent or received data
	{
		std::cout << "Error - SERVER ---> TCP ---> Lost connection to CLIENT\n";		// If the socket hasn't received data, print out an error indicating that the SERVER has lost connection to the CLIENT 
	}
	std::cout << "- SERVER ---> TCP ---> Send Status from SERVER: " << status << "\n";		// Output the SERVER Socket Status after information has been sent to CLIENT. Should output 0 in order for the data to have been sent fully
}
sf::Packet P2P::receiveTCPPacketClient(sf::TcpSocket &tcpS)
{
	sf::Packet packet;
		if (socketSelector.isReady(tcpS)) // checks if the tcp Socket is ready to receive data
		{
			sf::Socket::Status status = tcpS.receive(packet);
			if (status != sf::Socket::Done)
			{
				std::cout << "!!! Error --- PACKET ---> TCP ---> Cannot recieve data from SERVER !!!" << std::endl;
			}	
			std::cout << "--- PACKET ---> TCP ---> Receiving from SERVER status: " << status << std::endl;
		}
	return packet;
}
sf::Packet P2P::receiveTCPPacketServer(sf::TcpSocket& tcpS)
{
	sf::Packet packet;
	if (socketSelector.isReady(tcpS)) // checks if the tcp Socket is ready to receive data
	{
		const sf::Socket::Status status = tcpS.receive(packet);
		if (status != sf::Socket::Done)
		{
			std::cout << "!!! Error --- PACKET ---> TCP ---> Cannot receive data from CLIENT !!!\n";
		}
		std::cout << "--- PACKET ---> TCP ---> Receiving from CLIENT status: " << status << "\n";
	}
	return packet;
}

// --- UDP Setup ---
void P2P::udpBindServer() // setting server side to bind to port 53000
{
	if (getIsHost())
	{
		if (udpSocketServer.bind(53000) != sf::Socket::Done)
		{
			std::cout << "!!! Error - SERVER ---> UDP ---> Failed to bind Server to UPD port !!!\n";
		}
		//udpSocketServer.setBlocking(false);
		socketSelector.add(udpSocketServer);
	}
}
void P2P::udpBindClient() // binding UDP socket on the client side to any port available which will be chosen at random and unpredictable to determine which port it will be
{
	if (udpSocketClient.bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		std::cout << "!!! Error -- CLIENT ---> UDP ---> Failed to bind server to UPD port on client side !!!\n";
	}
	udpSocketClient.setBlocking(false);
	socketSelector.add(udpSocketClient);

	std::cout << "CLIENT UDP port: " << udpSocketClient.getLocalPort() << "\n";

	sf::Packet uDPPacket;
	if (!getIsHost())
	{
		
			tanks[0]->setPosition(windowWidth - 80, windowHeight / 2);
			tanks[0]->setRotation(goWest);
		
		
			//tanks[0]->setPosition(windowWidth / 2, 40);
			//tanks[0]->setRotation(goSouth);
			//
			//tanks[0]->setPosition(windowWidth / 2, windowHeight - 80);
			//tanks[0]->setRotation(goNorth);
		
	}
	const int tankX = tanks[0]->getPosition().x;
	const int tankY = tanks[0]->getPosition().y;
	uDPPacket << "PlayerJoined" << udpSocketClient.getLocalPort() << tankX << tankY;
	sendPacketClient(uDPPacket);
}

// --- UDP Sending/Recieving ---
void P2P::sendUDPPacketClient(sf::Packet p) // Send information from CLIENT to SERVER by passing through a packet in as an argument
{
	// --- Packet CLIENT side UDP: SENDING ---	
	sf::Socket::Status status = udpSocketClient.send(p, "localhost", 53000); // check status of CLIENT socket
	if (status != sf::Socket::Done)
	{
		std::cout << "!!! Error -- CLIENT ---> UDP ---> Status: " << status << " !!!" << std::endl;
	}
	//std::cout << "-- CLIENT ---> SENDING UDP ---> Status: " << status << std::endl;
}
void P2P::sendUDPPacketServer(sf::Packet p, Client* c) // Send information from SERVER to CLIENT by passing through a packet and the current client struct in as an argument
{
	// --- Packet SERVER side UDP: SENDING ---	
	sf::Socket::Status status = udpSocketServer.send(p, "localhost", c->port); // Create a new socket status variable and assign a UDP socket sending function to it bassing through the packet, the local host as the address and the port number of the current CLIENT
	if (status != sf::Socket::Done) // Check if the SERVER UDP socket status is not 0
	{
		std::cout << "!!! Error -- SERVER CAN'T RECIEVE DATA ---> UDP ---> Status: " << status << " !!!" << std::endl; // If the check is anything but 0 then print an Error to the console
	}
	//std::cout << "-- SERVER ---> UDP ---> Status: " << status << std::endl; // Output the Status which should be 0 to confirm that the SERVER has sent data
}
sf::Packet P2P::receiveUDPPacketClient() // Reiceive information from the SERVER to the CLIENT via UDP
{
	sf::IpAddress clientAddress; // Temp variable for the IP address the SERVER holds
	unsigned short clientPort; // Port number of the SERVER
	sf::Packet recieveUDPPacket; // Packet that will receive the CLIENT'S data
	//if (socketSelector.isReady(udpSocketClient)) // This function must be used after a call to wait, to know if the CLIENT socket is ready to receive data
	//{
		sf::Socket::Status status = udpSocketClient.receive(recieveUDPPacket, clientAddress, clientPort); // create a new status and assign the UDP CLIENT socket to it
		if (status != sf::Socket::Done) // check if the UDP socket CLIENT didn't receive any or all of the data
		{
			std::cout << "!!! Error -- CLIENT CAN'T RECIEVE DATA ---> UDP ---> Status: " << status << " !!!" << std::endl; // if the socket didn't receive or partially received the data, output error
		}
	//}
	return recieveUDPPacket; // return whatever was received 
}
sf::Packet P2P::receiveUDPPacketServer() // Reiceive information from the CLIENT to the SERVER via UDP
{
	sf::IpAddress serverAddress; // Temp variable for the IP address the CLIENT holds
	unsigned short serverPort; // Port number of the CLIENT
	sf::Packet recieveUDPPacket; // Packet that will receive the CLIENT'S data
	if (socketSelector.wait(sf::milliseconds(50)))
	{
		if (socketSelector.isReady(udpSocketServer)) // This function must be used after a call to wait, to know if the SERVER socket is ready to receive data
		{
			sf::Socket::Status status = udpSocketServer.receive(recieveUDPPacket, serverAddress, serverPort); // create a new status and assign the UDP SERVER socket to it
			if (status != sf::Socket::Done) // check if the UDP socket SERVER didn't receive any or all of the data
			{
				std::cout << "Error -- SERVER CAN'T RECIEVE DATA ---> UDP ---> Status: " << status << std::endl; // if the socket didn't receive or partially received the data, output error
			}
		}
	}
	return recieveUDPPacket; // return whatever was received 
}

// --- Prediction ---
Prediction* P2P::prediction(Tank* t)
{
	// S = ut + 1/2at^2

	if (t->predictions.size() < 2)
	{
		if (!t->predictions.empty())
		{
			return t->predictions.back();
		}
		return new Prediction(0, sf::Vector2f(0,0), sf::Vector2f(0,0));
	}
	Prediction& nextPrediction = *t->predictions.back();
	Prediction& prevPrediction = *t->predictions.at(t->predictions.size() - 2);

	float currentTime = nextPrediction.gameTime;
	float prevTime = prevPrediction.gameTime;

	float alpha = currentTime - prevTime;
	Prediction* prediction = new Prediction();
	prediction->tankPos.x = prevPrediction.tankPos.x + alpha * (nextPrediction.tankPos.x - prevPrediction.tankPos.x);
	prediction->tankPos.y = prevPrediction.tankPos.y + alpha * (nextPrediction.tankPos.y - prevPrediction.tankPos.y);

	return prediction;
}

void P2P::setIsHost(bool iH)
{
	isHost = iH;
}

// --- Getters ---
bool P2P::getIsHost()
{
	return isHost;
}
Tank* P2P::getTank(int iD)
{
	for (auto& t : tanks)
	{
		if (t->getTankID() == iD)
		{
			return t;
		}
	}

	return nullptr;
}
