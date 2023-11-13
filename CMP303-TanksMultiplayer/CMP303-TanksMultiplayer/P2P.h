#pragma once
#include "SFML/Network.hpp"
#include "Tank.h"
class P2P
{
private:
	// Networking variables
	sf::SocketSelector socketSelector;
	// setting up the TCP listener
	bool isHost = true; // bool to determine the host. if a host already exists then set to false and stop programme from listening for clients
	sf::TcpListener tcpListener;
	// setting up TCP socket
	sf::TcpSocket tcpSocket;
	// setting up packet
	sf::Packet packet;

	std::vector<Tank*> tank;

public:
	P2P(std::vector<Tank*> t);
	~P2P();
	void tcpListeningCheck();
	void tcpStatusCheck();
	void checkIsHost();
	bool getIsHost();
	void socketSelection();
};

