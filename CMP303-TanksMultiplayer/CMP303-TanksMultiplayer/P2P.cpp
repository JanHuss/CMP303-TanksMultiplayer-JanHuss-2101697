#include "P2P.h"



P2P::P2P(std::vector<Tank*> t) : tank(t)
{
	hasJoined = "A client has joined";
}

P2P::~P2P()
{
}

void P2P::tcpListeningCheck()
{
	if (tcpListener.listen(53000) != sf::Socket::Done)
	{
		printf("Failed to bind server to port\n");
		isHost = false;
	}
	else
	{
		socketSelector.add(tcpListener);
	}
}

void P2P::tcpStatusCheck() // attempts to connect to tcp socket
{
	sf::Socket::Status tcpSocketStatus = tcpSocket.connect("localhost", 53000/*,timeOut*/);

	if (tcpSocketStatus != sf::Socket::Done)
	{
		printf("Error binding TCP socket\n"); // error message if TCP socket fails to bind
	}

	printf("Socket is binding\n");
}

void P2P::checkIfThereIsHost() // checks if the the application is a host or a client
{
	if (isHost) // if the host is true, the application turns to be a client
	{
		if (socketSelector.wait(sf::milliseconds(1))) // returns as soon as at least one socket has some data available to be received.
		{
			if (socketSelector.isReady(tcpListener))	// This function must be used after a call to Wait, to know
														// which sockets are ready to receive data and will prevent 
														// blocking because there is data to be received
			{
				printf("Is Host\n");
				//std::vector<sf::TcpSocket*> client = ;
				Client* someClient = new Client;

				client.push_back(someClient);
				for (int i = 0; i < client.size(); i++) 
				{
					if (tcpListener.accept(*someClient->tcpID) != sf::Socket::Done) // function is in blocking mode until a connection is actually received. 
																					// so if not accepting, print error
					{
						printf("Failed to connect to client\n");
					}

				}
				sf::Packet packet;
				packet << hasJoined;
				sendPacket(packet);
				//printf("A client connected\n");
				tank.push_back(new Tank("Blue", 90));			
				tank[0]->setPosition(40, 480 / 2);
				tank.push_back(new Tank("Green", 270));
				tank[1]->setPosition(40, 640 / 2);
			}
		}
	}
	else
	{

	}
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
			tcpSocket.send(p); // sending package from the tcp Socket
		}
}

sf::Packet P2P::recievePacket()
{
	sf::Packet packet;

		if (socketSelector.isReady(tcpSocket)) // checks if the tcp Socket is ready to receive data
		{
			tcpSocket.receive(packet); // receiving a package from tcp Socket
			//packet >> hasJoined;
		}
	return packet;
}
