#include "P2P.h"

P2P::P2P(std::vector<Tank*> t) : tank(t)
{
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

void P2P::tcpStatusCheck()
{
	sf::Socket::Status tcpSocketStatus = tcpSocket.connect("localhost", 53000/*,timeOut*/);

	if (tcpSocketStatus != sf::Socket::Done)
	{
		printf("Error binding TCP socket\n"); // error message if TCP socket fails to bind
		//tcpSocket.send(packet);
		//tcpSocket.receive(packet);
	}

	printf("Socket is binding\n");
}

void P2P::checkIsHost()
{
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
				tank.push_back(new Tank("Blue", 90));
				
				//tank[0]->setPosition(40, 480 / 2);
			}
		}
	}
}

bool P2P::getIsHost()
{
	return isHost;
}

void P2P::socketSelection()
{
	//sf::Time timeOut;
	socketSelector.add(tcpSocket);
	tcpSocket.setBlocking(false);
	if (socketSelector.wait(sf::milliseconds(1)))
	{
		if (socketSelector.isReady(tcpSocket))
		{

		}
	}
}
