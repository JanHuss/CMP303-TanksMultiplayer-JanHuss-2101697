#pragma once
#include "SFML/Network.hpp"
#include "Tank.h"

struct Client // struct that holds all client data. All information that needs to be sent and recieved in one location
{
	sf::TcpSocket* tcpID;	// pointer for the tcp client socket. this is used to assign a tcpSocket element when pushing back a new tcpclient element in the client vector.
							// this functions as an ID to be passed on and determines who is which player
	float posX; // x position for client tank
	float posY; // y position for client tank
};

class P2P
{
private:
	// Networking variables
	sf::SocketSelector socketSelector;
	sf::TcpListener tcpListener; // setting up the TCP listener
	sf::TcpSocket tcpSocket; // setting up TCP socket

	bool isHost = true; // bool to determine the host. if a host already exists then set to false and stop programme from listening for clients
	std::string hasJoined;

	std::vector<Tank*> tank; 
	std::vector<Client*> client; // vector of client pointers that will be pushed back once a client joins at which point the client vector element is assigned to the client struct ID



public:
	P2P(std::vector<Tank*> t);
	~P2P();
	void tcpListeningCheck();
	void tcpStatusCheck();
	void checkIfThereIsHost();
	bool getIsHost();
	//void socketSelection();
	void sendPacket(sf::Packet p); // function to send package data
	sf::Packet recievePacket(); // funtion to recieve package data
};

