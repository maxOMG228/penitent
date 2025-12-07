#pragma once
#include <SFML/Graphics.hpp>
#include <Vector>
#include <iostream>

struct Bullet {
	sf::RectangleShape shape;
	sf::Sprite sprite;
	static sf::Texture texture;
	static bool isTextureLoaded;

	sf::Vector2f velocity;
	bool isActive;


	Bullet(float x, float y, float dirX, float dirY) {
	if (!isTextureLoaded) {
		if (!texture.loadFromFile("textures/projectiles/just_arrow.png")) {
			std::cerr << "Failed to load bullet texture!" << std::endl;
		}
		isTextureLoaded = true;
	}

	sprite.setTexture(texture);
	sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
	sprite.setPosition(x, y);
	sprite.setScale(2.f, 2.f);

	shape.setSize(sf::Vector2f(10.0f, 10.0f));
	shape.setOrigin(5.0f, 5.0f);
	shape.setPosition(x, y);
	shape.setFillColor(sf::Color::Transparent);

	float lenght = std::sqrt(dirX * dirX + dirY * dirY);
	float speed = 7.f;
	velocity = sf::Vector2f((dirX / lenght) * speed, (dirY / lenght) * speed);
	isActive = true;

	float angle = std::atan2(dirY, dirX) * 180.f / 3.14159f;

	sprite.setRotation(angle - 90.f);
	shape.setRotation(angle);

	}
	void update() {
		shape.move(velocity);
		sprite.setPosition(shape.getPosition());
	}
};

enum EnemyType {
	BaseEnemy,
	ArcherEnemy,
};

class Enemy
{
public:
	sf::RectangleShape Shape;
	sf::Texture texture;
	sf::Sprite sprite;
	bool isJumping;
	sf::Vector2f jumpDirection;
	sf::Clock jumpTimer;
	int roomID;
	float startX, startY;
	bool isEnemyAlive;
	int hp;
	int frameWidth;
	int frameHeight;
	float animTimer;

	EnemyType type;

	std::vector<Bullet> bullets;
	sf::Clock shootTimer;
	float shootInterval;

	Enemy(float x, float y, int room, EnemyType type);

	void update(sf::Vector2f playerPos);
};

