#pragma once
#include <vector>
#include <map>
#include "Room.h"
#include "Player.h"

class Dungeon {
public:
	std::vector<Room*> rooms;
	int currentRoomIndex;

	sf::Texture tileset;
	sf::RectangleShape doorTriggers[4];
	sf::Vector2f doorPositions[4];

	sf::Clock damageClock;

	float iFrameCooldown;

	sf::Sprite backgroundSprite;
	std::map<std::string, sf::Texture> bgTextures;

	Dungeon(float winW, float winH);

	Artifact* getRandomArtifact(float x, float y);

	void loadTextures();
	void generateRandomLevel(int roomCount);

	void update(Player& player, float winW, float winH);
	void draw(sf::RenderWindow& window);
};