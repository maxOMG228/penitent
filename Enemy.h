#pragma once
#include <SFML/Graphics.hpp>

enum EnemyType {
	BaseEnemy,
};

class Enemy
{
public:
	sf::RectangleShape Shape;
	int roomID;
	float startX, startY;
	bool isEnemyAlive;
	int hp;

	EnemyType type;

	Enemy(float x, float y, int room, EnemyType type);

	void update(sf::Vector2f playerPos);
};

