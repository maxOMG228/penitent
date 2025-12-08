#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.h"
#include "Chest.h"
#include "Artifact.h"

enum RoomType {
	Normal,
	Safe,
	Treasure,
	Boss,
};

class Room
{
public:

	std::vector<Enemy> enemies;
	std::vector<Artifact*> artifacts;
	std::vector<Chest*> chests;

	bool isCleared;
	int nextRoomIndex[4];
	RoomType type;

	float roomWidth;
	float roomHeight;

	sf::Sprite backgroundSprite;
	sf::Texture bakedTexture;

	sf::Vector2f spawnPosition;

	Room();

	~Room() {
		for (auto art : artifacts) delete art; // delete artifacts
		artifacts.clear();

		for (auto chest : chests) delete chest; // delete chests
		chests.clear();
	}

	void generateBackground(sf::Texture& tileset, float w, float h);

	void addEnemy(float x, float y, EnemyType type);

	void updateStatus();
};

