#pragma once
#include "SFML/Network.hpp"
#include "Tank.h"


struct Client // struct that holds all client data. All information that needs to be sent and recieved in one location
{
	Client(sf::TcpSocket* tcpID)
	{
		this->tcpID = tcpID;
	}
	int port;
	sf::TcpSocket* tcpID;	// pointer for the tcp client socket. this is used to assign a tcpSocket element when pushing back a new tcpclient element in the client vector.
							// this functions as an ID to be passed on and determines who is which player
	sf::Vector2f playerPos; // to store tanks coordinates
};

class P2P
{
private:
	// variables
	sf::SocketSelector socketSelector;
	sf::TcpListener tcpListener; // setting up the TCP listener
	sf::TcpSocket tcpSocket; // setting up TCP socket

	sf::UdpSocket udpSocketServer;
	sf::UdpSocket udpSocketClient;

	bool isHost = true; // bool to determine the host. if a host already exists then set to false and stop programme from listening for clients
	std::string hasJoined;

	std::vector<Tank*>* tank; 
	std::vector<Client*> client; // vector of client pointers that will be pushed back once a client joins at which point the client vector element is assigned to the client struct ID

public:
	P2P(std::vector<Tank*>* t);
	~P2P();

	void peerToPeerArchitecture();



	// --- TCP ---
	void tcpListeningCheck();
	void tcpStatusCheck();

	void sendPacketClient(sf::Packet p); // function to send package data
	void sendTCPPacketServer(sf::Packet p, Client* pCP);
	sf::Packet recieveTCPPacket(sf::TcpSocket& tcpS); // funtion to recieve package data

	// --- UDP ---
	void udpBindServer();
	void udpBindClient();

	void sendUDPPacketClient(sf::Packet p);
	void sendUDPPacketServer(sf::Packet p, Client* c);
	sf::Packet recieveUDPPacketClient();
	sf::Packet recieveUDPPacketServer();
	
	bool getIsHost();
};

