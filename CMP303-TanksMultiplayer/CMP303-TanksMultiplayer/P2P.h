#pragma once
#include "SFML/Network.hpp"
#include "sfml/Graphics.hpp"
#include "Tank.h"
#include "Framework/Input.h"

struct Client // Struct that holds all client data. All information that needs to be sent and received in one location
{
	Client(sf::TcpSocket* tcpID, int pID)
	{
		this->tcpID = tcpID;
		this->playerID = pID;
	}
	int playerID;		// Variable to assign the player ID	
	unsigned short port;
	sf::TcpSocket* tcpID;	// Pointer for the tcp client socket. this is used to assign a tcpSocket element when pushing back a new tcpClient element in the client vector.
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
	bool isPlayerTwo;
	bool isPlayerThree;
	bool isPlayerFour;
	int maxPlayers;
	std::string hasJoined;

	std::vector<Tank*>& tanks; // Vector pointer of tank pointers
	Input* input;

	int windowWidth;
	int windowHeight;

	// Tank rotation variables
	int goNorth;
	int goSouth;
	int goEast;
	int goWest;

public:
	P2P(std::vector<Tank*>& t, Input* in, int winW, int winH);
	~P2P();

	std::vector<Client*> client; // Vector of client pointers that will be pushed back once a client joins at which point the client vector element is assigned to the client struct ID
	std::vector<sf::Vector3f> locations;

	float currentTime;
	bool windowOpen;
	// --- Host/Client Architecture ---
	void HostClientArchitecture();	// Structure that runs the HOST/CLIENT networking system.
									// First the network will check if the there is a HOST/SERVER available.
									// If there isn't the application will take the HOST/SERVER role and initially 
	void serverSetup();				// connect with itself as a CLIENT.
	void clientSetup();				// Any further applications that join the HOST will then become CLIENTS	

	// --- TCP Setup ---
	void tcpListeningCheck();	// Listening for HOST. If none, then set this application to HOST
	void tcpStatusCheck();		// CLIENT attempts to connect to TCP socket

	// --- TCP Sending/Receiving ---
	void sendPacketClient(sf::Packet p); // Send package data from CLIENT
	void sendTCPPacketServer(sf::Packet p, const Client* pCP); // Send package data from SERVER
	sf::Packet receiveTCPPacketClient(sf::TcpSocket& tcpS); // Receive package data from SERVER
	sf::Packet receiveTCPPacketServer(sf::TcpSocket& tcpS); // Receive package data from CLIENT

	// --- UDP Setup ---
	void udpBindServer(); // Bind SERVER to UDP socket
	void udpBindClient(); // Bind CLIENT to UDP socket

	// --- UDP Sending/Receiving ---
	void sendUDPPacketClient(sf::Packet p); // Function to send UDP packet from CLIENT to SERVER
	void sendUDPPacketServer(sf::Packet p, Client* c); // Function to send UDP packet from SERVER to CLIENT
	sf::Packet receiveUDPPacketClient(); // Receive UDP packet information from SERVER
	sf::Packet receiveUDPPacketServer(); // Receive UDP packet information from CLIENT
	
	// --- Prediction ---
	Prediction* prediction(Tank* t);

	// --- Setters ---
	void setIsHost(bool iH);

	// --- Getters ---
	bool getIsHost();
	Tank* getTank(int iD);
	
};

