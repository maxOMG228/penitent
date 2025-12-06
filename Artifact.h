#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

class Player;

class Artifact
{
public:
	sf::Sprite sprite;
	sf::Texture texture;
	std::string name;
	bool isPickedUp;

	Artifact(float x, float y) {
		isPickedUp = false;
		sprite.setPosition(x, y);
	}

	virtual ~Artifact(){}

	virtual void apply(Player& player) = 0;

	void draw(sf::RenderWindow& window) {
		if (!isPickedUp) {
			window.draw(sprite);
		}
	}
};

class SpeedBoots : public Artifact {
public:
	SpeedBoots(float x, float y);
	void apply(Player& player) override;
};

class DamageRing : public Artifact {
public:
	DamageRing(float x, float y);
	void apply(Player& player) override;
};

class AttackSpeedCharm : public Artifact {
public:
	AttackSpeedCharm(float x, float y);
	void apply(Player& player) override;
};



