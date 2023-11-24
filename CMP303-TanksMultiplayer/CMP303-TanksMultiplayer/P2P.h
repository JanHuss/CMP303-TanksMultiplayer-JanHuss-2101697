#pragma once
#include "SFML/Network.hpp"
#include "sfml/Graphics.hpp"
#include "Tank.h"
#include "Framework/Input.h"

struct Client // Struct that holds all client data. All information that needs to be sent and recieved in one location
{
	Client(sf::TcpSocket* tcpID, int pID)
	{
		this->tcpID = tcpID;
		this->playerID = pID;
	}
	int playerID;		// Variable to assign the player ID	
	unsigned short port;
	sf::TcpSocket* tcpID;	// Pointer for the tcp client socket. this is used to assign a tcpSocket element when pushing back a new tcpclient element in the client vector.
							// this functions as an ID to be passed on and determines who is which player
	sf::Vector2f playerPos; // To store tanks coordinates
};

class P2P
{
private:
	// variables
	sf::SocketSelector socketSelector; // Setting up socket selector
	sf::TcpListener tcpListener; // Setting up the TCP listener
	sf::TcpSocket tcpSocket; // Setting up TCP socket

	sf::UdpSocket udpSocketServer; // UDP socket for SERVER
	sf::UdpSocket udpSocketClient; // UDP socket for CLIENT

	bool isHost; // Bool to determine the host. If a host already exists then set to false and stop programme from listening for clients
	std::string hasJoined;

	std::vector<Tank*>& tanks; // Vector pointer of tank pointers
	Input* input;

	int windowWidth;
	int windowHeight;

public:
	P2P(std::vector<Tank*>& t, Input* in, int winW, int winH);
	~P2P();

	std::vector<Client*> client; // Vector of client pointers that will be pushed back once a client joins at which point the client vector element is assigned to the client struct ID
	// --- Peer to Peer Architecture ---
	void HostClientArchitecture();	// Structure that runs the HOST/CLIENT networking system.
									// First the network will check if the there is a HOST/SERVER available.
									// If there isn't the application will take the HOST/SERVER role and initially 
	void serverSetup();				// connect with itself as a CLIENT.
	void clientSetup();				// Any further applications that join the HOST will then become CLIENTS	

	// --- TCP Setup ---
	void tcpListeningCheck();	// Listening for HOST. If none, then set this application to HOST
	void tcpStatusCheck();		// CLIENT attempts to connect to TCP socket

	// --- TCP Sending/Recieving ---
	void sendPacketClient(sf::Packet p); // Send package data from CLIENT
	void sendTCPPacketServer(sf::Packet p, Client* pCP); // Send package data from SERVER
	sf::Packet recieveTCPPacketClient(sf::TcpSocket& tcpS); // Recieve package data from SERVER
	sf::Packet recieveTCPPacketServer(sf::TcpSocket& tcpS); // Recieve package data from CLIENT

	// --- UDP Setup ---
	void udpBindServer(); // Bind SERVER to UDP socket
	void udpBindClient(); // Bind CLIENT to UDP socket

	// --- UDP Sending/Recieving ---
	void sendUDPPacketClient(sf::Packet p); // Function to send UDP packet from CLIENT to SERVER
	void sendUDPPacketServer(sf::Packet p, Client* c); // Function to send UDP packet from SERVER to CLIENT
	sf::Packet recieveUDPPacketClient(); // Recieve UDP packet information from SERVER
	sf::Packet recieveUDPPacketServer(); // Recieve UDP packet information from CLIENT
	
	// --- Setters ---
	void setIsHost(bool iH);

	// --- Getters ---
	bool getIsHost();
};

