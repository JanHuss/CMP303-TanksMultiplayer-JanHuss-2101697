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
	if (socketSelector.wait(sf::milliseconds(1))) // returns as soon as at least one socket has some data available to be received.
	{
		// --- Server Area ---
		serverSetup();

		// --- Client Area ---
		clientSetup();
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
		sf::TcpSocket* tempTCPSocket = new sf::TcpSocket;

		if (tcpListener.accept(*tempTCPSocket) != sf::Socket::Done) // function is in blocking mode until a connection is actually received. 
			// so if not accepting, print error
		{
			std::cout << "Error - SERVER ---> TCP ---> Failed to connect to client" << std::endl;
		}

		tempTCPSocket->setBlocking(false);
		socketSelector.add(*tempTCPSocket);
		Client* playerClientPacket = new Client(tempTCPSocket);

		client.push_back(playerClientPacket);
		std::cout << "- SERVER ---> TCP ---> A new client has connected\n" << std::endl;

		// --- Packet SERVER side TCP: SENDING ---
		sf::Packet packet;
		std::string clientEntersRoom = "--- SERVER TO CLIENT ---> PACKET ---> TCP ---> Hi I'm SERVER welcome new CLIENT";
		packet << clientEntersRoom;
		sendTCPPacketServer(packet, playerClientPacket);

		// Add Tank to game
		tank->push_back(new Tank("green", 270));
		//tank[0].
	}
	else	// if the socket selector is not ready, block and iterate through the client vector and assign the id to each client
			// the server will then utilise this to determine which player is which and will relay this over to the other clients
	{
		// --- Packet SERVER side TCP: RECIEVING ---
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
					unsigned short port;
					packet >> port;
					c->port = port;
					std::cout << "- SERVER ---> UDP ---> Client port: " << port << std::endl;
					sf::Packet udppacket;
					udppacket << "- SERVER ---> UDP ---> sending test packet ";
					sendUDPPacketServer(udppacket, c);
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
		sf::Packet receiveTCPPacket = recieveTCPPacket(tcpSocket);
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
void P2P::sendTCPPacketServer(sf::Packet p, Client* pCP)
{
	sf::Socket::Status status = pCP->tcpID->send(p);
	if (status != sf::Socket::Done)
	{
		std::cout << "Error - SERVER ---> TCP ---> Lost connection to CLIENT" << std::endl;
	}
	std::cout << "- SERVER ---> TCP ---> Send Status from SERVER: " << status << std::endl;
}
sf::Packet P2P::recieveTCPPacket(sf::TcpSocket &tcpS)
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
void P2P::sendUDPPacketServer(sf::Packet p, Client* c)
{
	// --- Packet CLIENT side UDP: SENDING ---	
	sf::Socket::Status status = udpSocketServer.send(p, "localhost", c->port);
	if (status != sf::Socket::Done)
	{
		std::cout << "Error -- CLIENT CAN'T RECIEVE DATA ---> UDP ---> Status: " << status << std::endl;
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