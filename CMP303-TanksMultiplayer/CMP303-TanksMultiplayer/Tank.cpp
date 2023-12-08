#include "Tank.h"
#include "P2P.h"

Tank::Tank(std::string color, const float setR, Input* in)
{
	// initialisation
	setRotate = setR;
	input = in;

	// rotation variables
	goNorth = 180;
	goSouth = 0;
	goEast = 270;
	goWest = 90;

	m_BodyTexture.loadFromFile("Assets/" + color + "Tank.png");
	m_BarrelTexture.loadFromFile("Assets/" + color + "Barrel.png");
	setTexture(m_BodyTexture);

	setOrigin(getTextureRect().width / 2, getTextureRect().height / 2);

	m_GhostSprite.setTexture(m_BodyTexture);
	m_GhostSprite.setColor(sf::Color(255, 255, 255, 128));
	m_GhostSprite.setOrigin(getTextureRect().width / 2, getTextureRect().height / 2);
	setGhostPosition( getPosition() );

	m_BarrelSprite.setTexture(m_BarrelTexture);
	m_BarrelSprite.setOrigin(6, 2);
	m_BarrelSprite.setPosition( getPosition() );

	// setting the initial rotation of the tank and barrel sprites
	m_BarrelSprite.setRotation(setRotate);
	setRotation(setRotate);

	acceleration = 40.0f;
}

Tank::~Tank()
{
	for (auto p: predictions)
	{
		if (p)
		{
			p = nullptr;
		}
	}
	if (input)
	{
		input = nullptr;
	}
}

//Sets the tank's position to the latest network position
void Tank::Update(float dt)
{
	if (m_Messages.size() < 1)
		return;
	TankMessage latestMessage = m_Messages.back();
	setPosition( latestMessage.x, latestMessage.y );
}

void Tank::handleInput(float dt, float currentTime, P2P& p2p)
{
	const sf::Vector2f previousPos = getPosition();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) // move left
	{
		setPosition(getPosition().x + (-acceleration * dt), getPosition().y);
		// set player rotation
		m_BarrelSprite.setRotation(goWest);
		setRotation(goWest);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) // move right
	{
		setPosition(getPosition().x + (acceleration * dt), getPosition().y);
		// set player rotation
		m_BarrelSprite.setRotation(goEast);
		setRotation(goEast);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) // move up
	{
		setPosition(getPosition().x, getPosition().y + (-acceleration * dt));
		// set player rotation
		m_BarrelSprite.setRotation(goNorth);
		setRotation(goNorth);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) // move down
	{
		setPosition(getPosition().x, getPosition().y + (acceleration * dt));
		// set player rotation
		m_BarrelSprite.setRotation(goSouth);
		setRotation(goSouth);
	}
	const sf::Vector2f distance = previousPos - getPosition();
	
	// S = ut + 1/2at^2
	// S = displacement
	//sf::Vector2f displacement = speed * presentTime + 0.5 * acceleration * (presentTime) ^ 2;

	sf::Packet movementPacket;
	const std::string playerMovement = "PlayerMovement";
	movementPacket << playerMovement << getPosition().x << getPosition().y << getRotation() << distance.x << distance.y << tankID;
	p2p.sendUDPPacketClient(movementPacket);
}

//Use this to set the predictions position
void Tank::setGhostPosition( sf::Vector2f pos ) {
	m_GhostSprite.setPosition( pos );
}

void Tank::setTankID(int tID)
{
	tankID = tID;
}

int Tank::getTankID()
{
	return tankID;
}

//Add a message to the tank's network message queue
void Tank::AddMessage(const TankMessage & msg) {
	m_Messages.push_back(msg);
}

//This method calculates and stores the position, but also returns it immediately for use in the main loop
//This is my where predictions would be... IF I HAD ANY
sf::Vector2f Tank::RunPrediction(float gameTime) {
	float predictedX = -1.0f;
	float predictedY = -1.0f;

	const int msize = m_Messages.size();
	if( msize < 3 ) {
		return sf::Vector2f( predictedX, predictedX );
	}
	const TankMessage& msg0 = m_Messages[msize - 1];
	const TankMessage& msg1 = m_Messages[msize - 2];
	const TankMessage& msg2 = m_Messages[msize - 3];
	
	// FIXME: Implement predictions here!
	// You have:
	// - the history of position messages received, in "m_Messages"
	//   (msg0 is the most recent, msg1 the 2nd most recent, msg2 the 3rd most recent)
	// - the current time, in "gameTime"
	//
	// You need to update:
	// - the predicted position at the current time, in "predictedX" and "predictedY"
		
	return sf::Vector2f( predictedX, predictedY );
}

void Tank::Reset() {
	m_Messages.clear();
}

//Draw the tank / or the ghost / or both
const void Tank::Render(sf::RenderWindow* window) {
	if (m_RenderMode > 0)
		window->draw(m_GhostSprite);
	if (m_RenderMode != 1) {
		window->draw(*this);
		//window->draw(m_BarrelSprite);
	}
}
