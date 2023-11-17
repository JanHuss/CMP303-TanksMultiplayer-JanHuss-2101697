#include "P2P.h"

P2P::P2P(std::vector<Tank*> t) : tank(t)
{
	hasJoined = "A client has joined";
}

P2P::~P2P()
{
}

void P2P::tcpListeningCheck()// SERVER looking/listening for available socket
{
	if (tcpListener.listen(53000) != sf::Socket::Done)
	{
		std::cout << "Error - SERVER --- TCP ---> Failed to bind server to TCP port" << std::endl;
		isHost = false;
	}
	socketSelector.add(tcpListener);	
}

void P2P::tcpStatusCheck() // CLIENT - attempts to connect to tcp socket/// 
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

void P2P::udpBindServer() // setting server side to bind to port 53000
{
	if (udpSocketServer.bind(53000) != sf::Socket::Done)
	{
		std::cout << "Error - SERVER ---> UDP ---> Failed to bind Server to UPD port" << std::endl;
	}
	socketSelector.add(udpSocketServer);
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
	sendPacket(uDPPacket);
}



void P2P::checkIfThereIsHost() // checks if the the application is a host or a client
{
	// server area
	 // if the host is true, the application turns to be a client
	if (socketSelector.wait(sf::milliseconds(1))) // returns as soon as at least one socket has some data available to be received.
	{
		if (socketSelector.isReady(tcpListener) && isHost)	// This function must be used after a call to Wait, to know
															// which sockets are ready to receive data and will prevent 
															// blocking because there is data to be received
		{
			std::cout << "----------------------------------------------------------------------------------------" << std::endl;
			std::cout << "--- Is Host ---\n" << std::endl;
			sf::TcpSocket* tempTCPSocket = new sf::TcpSocket;

			if (tcpListener.accept(*tempTCPSocket) != sf::Socket::Done) // function is in blocking mode until a connection is actually received. 
																		// so if not accepting, print error
			{
				std::cout << "Error - SERVER ---> TCP ---> Failed to connect to client" << std::endl;
			}

			// TCP Section --------------------------------------------------------------------------------------------------------
			tempTCPSocket->setBlocking(false);
			socketSelector.add(*tempTCPSocket);
			Client* playerClientPacket = new Client(tempTCPSocket);

			client.push_back(playerClientPacket);
			std::cout << "- SERVER ---> TCP ---> A new client has connected\n" << std::endl;

			sf::Packet packet; 
			std::string clientEntersRoom = "- SERVER ---> PACKET ---> TCP ---> A Player has entered the Room";
			packet << clientEntersRoom;
		
			sf::Socket::Status status = playerClientPacket->tcpID->send(packet);
			if (status != sf::Socket::Done)
			{
				std::cout << "Error - SERVER ---> TCP ---> Lost connection" << std::endl;
			}
			std::cout << "- SERVER ---> TCP ---> Send Status: " << status << std::endl;
			
			//tank.push_back(new Tank("Blue", 90));			
			//tank[0]->setPosition(40, 480 / 2);
		}
		else	// if the socket selector is not ready, block and iterate through the client vector and assign the id to each client
				// the server will then utilise this to determine which player is which and will relay this over to the other clients
		{
			sf::Packet packet;
			for (auto& c : client)
			{
				if (socketSelector.isReady(*c->tcpID))
				{
					sf::Socket::Status status = c->tcpID->receive(packet);
					if (status != sf::Socket::Done)
					{
						std::cout << "Error - SERVER ---> TCP ---> Status: " << status << std::endl;
					}
					
					std::string clientEntersRoom;
					packet >> clientEntersRoom;
					std::cout << clientEntersRoom << std::endl;
					if (clientEntersRoom._Equal("PlayerJoined"))
					{
						int port;
						packet >> port;
						c->port = port;
						std::cout << "- SERVER ---> UDP ---> Client port: " << port << std::endl;
					}
					
				}
				// UDP Section -------------------------------------------------------------------------------------
				if (socketSelector.isReady(udpSocketServer))
				{
					sf::Packet packet;
					sf::IpAddress clientAddress;
					unsigned short clientPort;
					udpSocketServer.receive(packet, clientAddress, clientPort);
					std::string clientData;
					packet >> clientData;

					std::cout << clientData << std::endl;
				}
			}
		}
		if (socketSelector.isReady(udpSocketClient))
		{
			std::cout << "- SERVER ---> UDP ---> Socket is ready on CLIENT Side" << std::endl;
		}

		// client area---------------------------------------------------------------------------------------------
		if (socketSelector.isReady(tcpSocket))
		{
			if (!getIsHost()) 
			{
			std::cout << "----------------------------------------------------------------------------------------" << std::endl;
			std::cout << "--- Is Client ---\n" << std::endl;
			}

			sf::Packet packet;
			packet = recievePacket(tcpSocket);
			std::string clientEntersRoom;
			clientEntersRoom = "-- CLIENT ---> PACKET ---> Oh a new player connected!\n";
			packet >> clientEntersRoom;
			std::cout << clientEntersRoom << std::endl;

			// sending udp packet
			sf::Packet udpPacket;
			udpPacket << "-- CLIENT ---> PACKET ---> Hello world!\n";
			sf::Socket::Status status = udpSocketClient.send(udpPacket, "localhost", 53000);
			if (status != sf::Socket::Done)
			{
				//printf("Error status: " + status);
				std::cout << "Error -- CLIENT ---> UDP ---> Status: " << status << std::endl;
			}
			std::cout << "-- CLIENT ---> UDP ---> Status: " << status << std::endl;
		}
	}
}

bool P2P::getIsHost()
{
	return isHost;
}

void P2P::sendPacket(sf::Packet p)
{
		if (socketSelector.isReady(tcpSocket)) // checks if the tcp Socket is ready to send data
		{
			if (tcpSocket.send(p) != sf::Socket::Done) 
			{
				std::cout << "Error --- PACKET ---> TCP ---> Cannot send data" << std::endl; 
			}	
		}
}

sf::Packet P2P::recievePacket(sf::TcpSocket &tcpS)
{
	sf::Packet packet;
		if (socketSelector.isReady(tcpS)) // checks if the tcp Socket is ready to receive data
		{
			sf::Socket::Status status = tcpS.receive(packet);
			if (status != sf::Socket::Done)
			{
				std::cout << "Error --- PACKET ---> TCP ---> Cannot recieve data" << std::endl;
			}
			std::cout << "--- PACKET ---> TCP ---> Receive status: " << status << std::endl;
		}
	return packet;
}
