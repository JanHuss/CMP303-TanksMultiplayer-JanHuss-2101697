#include "P2P.h"

P2P::P2P(std::vector<Tank*> t) : tank(t)
{
	hasJoined = "A client has joined";
}

P2P::~P2P()
{
}

void P2P::tcpListeningCheck()// server
{
	if (tcpListener.listen(53000) != sf::Socket::Done)
	{
		std::cout << "Failed to bind server to TCP port" << std::endl;
		isHost = false;
	}
	socketSelector.add(tcpListener);
	
}
void P2P::tcpStatusCheck() // attempts to connect to tcp socket/// client
{
	sf::Socket::Status tcpSocketStatus = tcpSocket.connect("localhost", 53000);
	tcpListener.setBlocking(false);
	socketSelector.add(tcpSocket);
	if (tcpSocketStatus != sf::Socket::Done)
	{
		printf("Error binding TCP socket\n"); // error message if TCP socket fails to bind
	}

	printf("TCP socket is binding\n");
}

void P2P::udpBindServer() // setting server side to bind to port 53000
{
	if (udpSocketServer.bind(53000) != sf::Socket::Done)
	{
		std::cout << "Failed to bind server to UPD port on server side" << std::endl;
	}
	socketSelector.add(udpSocketServer);
}

void P2P::udpBindClient() // binding UDP socket on the client side to any port available which will be chosen at random and unpredictable to determine which port it will be
{
	if (udpSocketClient.bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		std::cout << "Failed to bind server to UPD port on client side" << std::endl;
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
			printf("Is Host\n");
			sf::TcpSocket* tempTCPSocket = new sf::TcpSocket;

			if (tcpListener.accept(*tempTCPSocket) != sf::Socket::Done) // function is in blocking mode until a connection is actually received. 
				// so if not accepting, print error
			{
				printf("Failed to connect to client\n");
			}
			//else // if it doos accept, the server will call itself to be a client
			//{
				// TCP Section
				tempTCPSocket->setBlocking(false);
				socketSelector.add(*tempTCPSocket);
				Client* playerClientPacket = new Client(tempTCPSocket);

				client.push_back(playerClientPacket);

				printf("A new client has connected\n");
				sf::Packet packet; //create a new packet
				std::string clientEntersRoom = "A Player has entered the Room";
				packet << clientEntersRoom;
				//packet << "1"; // then send a string to identify which client is which
				//packet << playerClientPacket->playerPos;
				sf::Socket::Status status = playerClientPacket->tcpID->send(packet);
				if (status != sf::Socket::Done)
				{
					printf("Error has lost connection");
				}
				std::cout << "TCP send Status" << (status) << std::endl;
			//}
			//tank.push_back(new Tank("Blue", 90));			
			//tank[0]->setPosition(40, 480 / 2);
			//playerClientPacket->playerPos = tank[0]->getPosition(); // store the tank position into a vector2f
			//std::cout << "tank vec size: " + tank.size();



			//tank.push_back(new Tank("Green", 270));
			//tank[1]->setPosition(40, 640 / 2);


			//printf("A client connected\n");

		}
		else // if the socket selector is not ready, block and iterate through the client vector and assign the id to each client
			// the server will then utilise this to determine which player is which and will relay this over to the other clients
		{
			//if (socketSelector.wait(sf::milliseconds(1))) // returns as soon as at least one socket has some data available to be received.
			//{
			sf::Packet packet;
			for (auto& c : client)
			{
				if (socketSelector.isReady(*c->tcpID))
				{
					sf::Socket::Status status = c->tcpID->receive(packet);
					if (status != sf::Socket::Done)
					{
						//printf("Error status: " + status);
						std::cout << "Error - status: " << status << std::endl;
					}
					//recievePacket(packet);
					std::string clientEntersRoom;
					packet >> clientEntersRoom;
					std::cout << clientEntersRoom << std::endl;
					if (clientEntersRoom._Equal("PlayerJoined"))
					{
						int port;
						packet >> port;
						c->port = port;
						std::cout << "UDP Client port: " << port << std::endl;
					}
					
				}
					// UDP Section
				if (socketSelector.isReady(udpSocketServer))
				{
					sf::Packet packet;
					sf::IpAddress clientAddress;
					unsigned short clientPort;
					udpSocketServer.receive(packet, clientAddress, clientPort);
					std::string data;
					packet >> data;

					std::cout << data << std::endl;
				}
			}
			//}
		}
			if (socketSelector.isReady(udpSocketClient))
			{
				std::cout << "UDP Socket is ready on CLIENT Side" << std::endl;
			}
		// client area
		if (socketSelector.isReady(tcpSocket))
		{
			sf::Packet packet;
			packet = recievePacket(tcpSocket);
			std::string clientEntersRoom;
			clientEntersRoom = "oh a new player connected!";
			packet >> clientEntersRoom;
			std::cout << clientEntersRoom << std::endl;

			// sending udp packet
			sf::Packet udpPacket;
			udpPacket << "hello world";
			sf::Socket::Status status = udpSocketClient.send(udpPacket, "localhost", 53000);
			if (status != sf::Socket::Done)
			{
				//printf("Error status: " + status);
				std::cout << "Error - status: " << status << std::endl;
			}
			std::cout << "Status: " << status << std::endl;
		}
	}
	//else if (!isHost)
	//{
	//	if (socketSelector.wait(sf::milliseconds(1))) // returns as soon as at least one socket has some data available to be received.
	//	{
	//		if (socketSelector.isReady(tcpListener))	// This function must be used after a call to Wait, to know
	//													// which sockets are ready to receive data and will prevent 
	//													// blocking because there is data to be received
	//		{
	//			printf("Is Client\n");
	//
	//			tank.push_back(new Tank("Blue", 90));
	//			tank[0]->setPosition(40, 480 / 2);
	//			//playerClientPacket->playerPos = tank[0]->getPosition(); // store the tank position into a vector2f
	//			//tank.push_back(new Tank("Green", 270));
	//			//tank[1]->setPosition(40, 640 / 2);
	//			sf::Packet packet;
	//			recievePacket(packet);
	//		}
	//	}
	//}
}

bool P2P::getIsHost()
{
	return isHost;
}

//void P2P::socketSelection()
//{
//	//sf::Time timeOut;
//	socketSelector.add(tcpSocket);
//	tcpSocket.setBlocking(false);
//	if (socketSelector.wait(sf::milliseconds(1)))
//	{
//		if (socketSelector.isReady(tcpSocket))
//		{
//			tcpSocket.receive(packet);
//			packet >> hasJoined;
//		}
//	}
//}

void P2P::sendPacket(sf::Packet p)
{
		if (socketSelector.isReady(tcpSocket)) // checks if the tcp Socket is ready to send data
		{
			if (tcpSocket.send(p) != sf::Socket::Done) 
			{
				std::cout << "Error sending data" << std::endl; 
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
				printf("Error lost connection");
			}
			std::cout << "TCP receive status: " << status << std::endl;

		}
	return packet;
}
