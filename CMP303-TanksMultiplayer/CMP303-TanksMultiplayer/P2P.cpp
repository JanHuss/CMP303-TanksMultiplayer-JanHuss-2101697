#include "P2P.h"

P2P::P2P(std::vector<Tank*>* t) : tank(t)
{
	hasJoined = "A client has joined";
}
P2P::~P2P()
{
}

// --- Peer to Peer Architecture ---
void P2P::peerToPeerArchitecture()
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
	if (socketSelector.isReady(tcpListener) && isHost)	// This function must be used after a call to Wait, to know
														// which sockets are ready to receive data and will prevent 
														// blocking because there is data to be received.
														// Also, if host is true, the application turns to be a client
	{
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << "--------------------------------- Server -----------------------------------\n" << std::endl;
		// TCP Section -----------------------------------------------------------------------------------------------
		sf::TcpSocket* tempTCPSocket = new sf::TcpSocket; // create a new temporary TCP socket pointer

		if (tcpListener.accept(*tempTCPSocket) != sf::Socket::Done) //If the socket is in blocking mode, this function will not return until a connection is actually received.
		{
			std::cout << "!!! Error - SERVER ---> TCP ---> Failed to connect to client !!!" << std::endl;	// If not accepting the temporary TCP socket pointer,
		}																							// print error indicating that the TCP socket pointer failed to connect to CLIENT

		tempTCPSocket->setBlocking(false); // Set blocking to false. Calls will always return immediately, using the return code to signal whether there was data available or not.
		socketSelector.add(*tempTCPSocket); // Add the temporary TCP socket pointer to the socket selector, which is a multiplexer that allows to read from multiple sockets. 
		Client* playerClient = new Client(tempTCPSocket); // Create a CLIENT struct pointer and assign it a new CLIENT struct, passing through the temp TCP socket essentially assigning a socket to the client

		client.push_back(playerClient); // Add a the newly created CLIENT struct with the assigned temp TCP socket to the client struct vector. This means that a player has connected with the HOST/SERVER
										// and will be used to recieve information from the CLIENTS but also to send data back out to the CLIENTS
		std::cout << "- SERVER ---> TCP ---> A new CLIENT has connected\n" << std::endl; // Message that a CLIENT has connected
		std::cout << "&&& Number of CLIENTS connected: " << client.size() << " &&&\n" << std::endl; // Message to show how many CLIENTS have joined the network

		// --- Packet SERVER side TCP: SENDING ---
		sf::Packet packet; // Create a packet
		std::string clientEntersRoom = "--- SERVER TO CLIENT ---> PACKET ---> TCP ---> Hi I'm SERVER welcome new CLIENT"; // Create a test string with a message to put into packet
		packet << clientEntersRoom; // Pack the test string into the packet
		sendTCPPacketServer(packet, playerClient); // send the packet allong with the playerClient pointer containing the temp TCP socket to the CLIENT

		// Add Tank to game
		tank->push_back(new Tank("green", 270));
		//tank[0].
	}
	else	// if the socket selector is not ready, block and iterate through the client vector and assign the ID to each client
			// the server will then utilise this to determine which player is which and will relay this over to the other clients
	{
		// --- Packet SERVER side TCP: RECIEVING ---
		sf::Packet packet; // Create a new packet for the server to receive information from a CLIENT
		for (auto& c : client) // Iterate throught the client struct vector. Every c in client resembles a CLIENT/Player
		{
			if (socketSelector.isReady(*c->tcpID))	// This function must be used after a call to Wait, to know which sockets are ready to receive data. 
			{										//If a socket is ready, a call to receive will never block because we know that there is data available to read.
				sf::Socket::Status status = c->tcpID->receive(packet); // Create a new socket status set to give a status on whether the CLIENT packet information has been received
				if (status != sf::Socket::Done) // Check that the status of the recieved packet is not 0. Otherwise print out an Error
				{
					std::cout << "!!! Error - SERVER ---> TCP ---> Status: " << status << " !!!" << std::endl; // Print Error if the status is not 0
				}
				// Initially Binding SERVER UDP
				std::string clientEntersRoom; // Create a string for the receiving packet to unload on
				packet >> clientEntersRoom; // Pass packet information to string
				std::cout << clientEntersRoom << std::endl; // Output received data which should be PlayerJoined

				if (clientEntersRoom._Equal("PlayerJoined")) // Check if the string is equal to the string of the socket that binds the UDP SERVER function
				{
					unsigned short port; // create an unsigned short variable for the received UDP packet to pass data to
					packet >> port; // pass data from received packet to the unsigned short variable
					c->port = port; // assign the unsigned short variable to the current CLIENT'S port variable
					std::cout << "- SERVER ---> UDP ---> Client port: " << port << std::endl; // Output the current CLIENT'S port number
					sf::Packet udppacket; // Create a new packet that can be sent via UDP
					udppacket << "- SERVER ---> UDP ---> sending test packet "; // Pass in string to send a test message


					sendUDPPacketServer(udppacket, c); // Pass back informtation via UDP from the information received by the other CLIENTS???
				}
			}
			// UDP Section -------------------------------------------------------------------------------------------
			if (socketSelector.isReady(udpSocketServer))
			{
				// --- Packet SERVER side UDP: RECIEVING --- 
				std::string clientEntersRoom;
				std::string clientData;
				sf::Packet uDPPacket;
				uDPPacket = recieveUDPPacketServer();
				uDPPacket >> clientData;
				std::cout << "Client data: " << clientData << std::endl;
			}
		}
	}
}
void P2P::clientSetup()
{
	// ------------------------------------------------ Client Area ------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------
	//if (socketSelector.wait(sf::milliseconds(1)))
		//{
	if (socketSelector.isReady(tcpSocket))
	{
		if (!getIsHost())
		{
			std::cout << "----------------------------------------------------------------------------" << std::endl;
			std::cout << "--------------------------------- Client -----------------------------------\n" << std::endl;
		}

		// --- Packet CLIENT side TCP: SENDING ---
		sf::Packet sendTCPPacket;
		sendTCPPacket << "--- CLIENT TO SERVER ---> PACKET ---> TCP ---> Hi, I'm A CLIENT. I hope you don't mind me joining =)\n";
		sendPacketClient(sendTCPPacket);

		// --- Packet CLIENT side TCP: RECIEVING --- 
		sf::Packet receiveTCPPacket = recieveTCPPacketClient(tcpSocket);
		std::string clientEntersRoom;

		receiveTCPPacket >> clientEntersRoom;
		std::cout << clientEntersRoom << std::endl;

		// add client tank
		if (!isHost)
		{
			tank->push_back(new Tank("blue", 90));
			tank->push_back(new Tank("green", 270));
		}
	}
	if (socketSelector.isReady(udpSocketClient))
	{

		// --- PackeT CLIENT side UDP: RECIEVING ---			
		std::string clientData;
		recieveUDPPacketClient() >> clientData;;
		std::cout << clientData << std::endl;

		// --- Packet CLIENT SIDE UDP: INIT SEND ---
		sf::Packet udpPacket;
		udpPacket << "--- CLIENT TO SERVER ---> PACKET ---> UDP ---> Hello SERVER! I'm a CLIENT sending a message over UDP\n";
		sendUDPPacketClient(udpPacket);
	}
	//}
}

// --- TCP Setup ---
void P2P::tcpListeningCheck()// SERVER looking/listening for available socket
{
	if (tcpListener.listen(53000) != sf::Socket::Done)
	{
		std::cout << "Error - SERVER --- TCP ---> Failed to bind server to TCP port" << std::endl;
		isHost = false;
	}
	
	socketSelector.add(tcpListener);	
}
void P2P::tcpStatusCheck() // CLIENT - attempts to connect to tcp socket
{
	sf::Socket::Status tcpSocketStatus = tcpSocket.connect("localhost", 53000);
	tcpListener.setBlocking(false);
	socketSelector.add(tcpSocket);
	if (tcpSocketStatus != sf::Socket::Done)
	{
		std::cout << "Error -- CLIENT --- TCP ---> Cannot bind to TCP socket" << std::endl; // error message if TCP socket fails to bind
	}

	std::cout << "-- CLIENT --- TCP ---> TCP socket is binding" << std::endl;
}

// --- TCP Sending/Recieving ---
void P2P::sendPacketClient(sf::Packet p)
{
		//if (socketSelector.isReady(tcpSocket)) // checks if the tcp Socket is ready to send data
		//{
			if (tcpSocket.send(p) != sf::Socket::Done) 
			{
				std::cout << "Error --- PACKET ---> TCP ---> Cannot send data" << std::endl; 
			}	
		//}
}
void P2P::sendTCPPacketServer(sf::Packet p, Client* pCP)	// Send a packet from the SERVER to the CLIENT with the dedicated ID which is being passed through as a pCP argument
{
	sf::Socket::Status status = pCP->tcpID->send(p);	// Create a status variable and assign the CLIENT ID send information to it
	if (status != sf::Socket::Done)		// Utilising the CLIENT ID send infomation, check if the socket hasn't sent or received data
	{
		std::cout << "Error - SERVER ---> TCP ---> Lost connection to CLIENT" << std::endl;		// If the socket hasn't received data, print out an error indicating that the SERVER has lost connection to the CLIENT 
	}
	std::cout << "- SERVER ---> TCP ---> Send Status from SERVER: " << status << std::endl;		// Output the SERVER Socket Status after information has been sent to CLIENT. Should output 0 in order for the data to have been sent fully
}
sf::Packet P2P::recieveTCPPacketClient(sf::TcpSocket &tcpS)
{
	sf::Packet packet;
		if (socketSelector.isReady(tcpS)) // checks if the tcp Socket is ready to receive data
		{
			sf::Socket::Status status = tcpS.receive(packet);
			if (status != sf::Socket::Done)
			{
				std::cout << "Error --- PACKET ---> TCP ---> Cannot recieve data from SERVER" << std::endl;
			}
			std::cout << "--- PACKET ---> TCP ---> Receiving from SERVER status: " << status << std::endl;
		}
	return packet;
}
// --- NOT BEING USED ATM ---
sf::Packet P2P::recieveTCPPacketServer(sf::TcpSocket& tcpS)
{
	sf::Packet packet;
	if (socketSelector.isReady(tcpS)) // checks if the tcp Socket is ready to receive data
	{
		sf::Socket::Status status = tcpS.receive(packet);
		if (status != sf::Socket::Done)
		{
			std::cout << "Error --- PACKET ---> TCP ---> Cannot recieve data from CLIENT" << std::endl;
		}
		std::cout << "--- PACKET ---> TCP ---> Receiving from CLIENT status: " << status << std::endl;
	}
	return packet;
}
// --- NOT BEING USED ATM ---

// --- UDP Setup ---
void P2P::udpBindServer() // setting server side to bind to port 53000
{
	if (!getIsHost())
	{
		if (udpSocketServer.bind(53000) != sf::Socket::Done)
		{
			std::cout << "Error - SERVER ---> UDP ---> Failed to bind Server to UPD port" << std::endl;
		}
		udpSocketServer.setBlocking(false);
		socketSelector.add(udpSocketServer);
	}
}
void P2P::udpBindClient() // binding UDP socket on the client side to any port available which will be chosen at random and unpredictable to determine which port it will be
{
	if (udpSocketClient.bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		std::cout << "Error -- CLIENT ---> UDP ---> Failed to bind server to UPD port on client side" << std::endl;
	}
	udpSocketClient.setBlocking(false);
	socketSelector.add(udpSocketClient);
	sf::Packet uDPPacket;
	uDPPacket << "PlayerJoined" << udpSocketClient.getLocalPort();
	sendPacketClient(uDPPacket);
}

// --- UDP Sending/Recieving ---
void P2P::sendUDPPacketClient(sf::Packet p)
{
	// --- Packet CLIENT side UDP: SENDING ---	
	sf::Socket::Status status = udpSocketClient.send(p, "localhost", 53000);
	if (status != sf::Socket::Done)
	{
		std::cout << "Error -- CLIENT ---> UDP ---> Status: " << status << std::endl;
	}
	std::cout << "-- CLIENT ---> SENDING UDP ---> Status: " << status << std::endl;
}
void P2P::sendUDPPacketServer(sf::Packet p, Client* c) // Send information from SERVER to CLIENT by passing through a packet and the current client struct in as an argument
{
	// --- Packet SERVER side UDP: SENDING ---	
	sf::Socket::Status status = udpSocketServer.send(p, "localhost", c->port); // Create a new socket status variable and assign a UDP socket sending function to it bassing through the packet, the local host as the address and the port number of the current CLIENT
	if (status != sf::Socket::Done) // Check if the SERVER UDP socket status is not 0
	{
		std::cout << "Error -- CLIENT CAN'T RECIEVE DATA ---> UDP ---> Status: " << status << std::endl; // If the check is anything but 0 then print an Error to the console
	}
	std::cout << "-- SERVER ---> UDP ---> Status: " << status << std::endl;
}
sf::Packet P2P::recieveUDPPacketClient()
{
	sf::IpAddress clientAddress;
	unsigned short clientPort;
	sf::Packet recieveUDPPacket;
	if (socketSelector.isReady(udpSocketClient)) 
	{
		sf::Socket::Status status = udpSocketClient.receive(recieveUDPPacket, clientAddress, clientPort);
		if (status != sf::Socket::Done)
		{
			std::cout << "Error -- CLIENT CAN'T RECIEVE DATA ---> UDP ---> Status: " << status << std::endl;
		}
	}
	return recieveUDPPacket;
}
sf::Packet P2P::recieveUDPPacketServer()
{
	sf::IpAddress serverAddress;
	unsigned short serverPort;
	sf::Packet recieveUDPPacket;
	if (socketSelector.isReady(udpSocketServer))
	{
		sf::Socket::Status status = udpSocketServer.receive(recieveUDPPacket, serverAddress, serverPort);
		if (status != sf::Socket::Done)
		{
			std::cout << "Error -- SERVER CAN'T RECIEVE DATA ---> UDP ---> Status: " << status << std::endl;
		}
	}
	return recieveUDPPacket;
}

// --- Getters ---
bool P2P::getIsHost()
{
	return isHost;
}