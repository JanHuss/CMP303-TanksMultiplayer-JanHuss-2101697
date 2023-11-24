#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <vector>
#include "TankMessage.h"

#include "Framework/Input.h"
#include <iostream>

class P2P;

class Tank : public sf::Sprite
{
public:
	Tank(std::string color, int setR, Input* in);
	~Tank();

	enum RenderMode {
		REAL_ONLY,
		PREDICTED_ONLY,
		REAL_AND_PREDICTED
	};

	void Update(float dt);
	void handleInput(float dt, P2P& p2p);
	const void Render(sf::RenderWindow* window);

	void AddMessage(const TankMessage& msg);
	sf::Vector2f RunPrediction(float gameTime);

	void SetRenderMode(const RenderMode renderMode) { m_RenderMode = renderMode; }
	//void setPosition( float x, float y );
	void setGhostPosition( sf::Vector2f pos );
	void Reset();

private:
	sf::Sprite	m_BarrelSprite;
	sf::Sprite	m_GhostSprite;

	sf::Texture m_BodyTexture;
	sf::Texture m_BarrelTexture;
	float		m_BodyRotation;
	float		m_BarrelRotation;

	RenderMode	m_RenderMode = RenderMode::REAL_AND_PREDICTED;

	Input* input;

	std::vector<TankMessage> m_Messages;

	float speed;
	int setRotate;
	int goNorth;
	int goSouth;
	int goEast;
	int goWest;
};

