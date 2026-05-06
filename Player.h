#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:

	// animation
	int frameWidth;
	int frameHeight;
	int currentFrame;
	int numFrames;

	sf::Clock animTimer;
	float timePerFrame;

	sf::RectangleShape hitbox;
	sf::RectangleShape swordHitbox;
	sf::Texture attackTexture;
	sf::Sprite attackSprite;

	sf::Texture texture;
	sf::Sprite sprite;

	int hp;
	int maxHp;
	float speed;
	int damage;

	bool isRolling;
	sf::Vector2f rollDirection;
	float rollSpeed;
	sf::Clock rollTimer;
	float rollDuration;

	sf::Clock rollCooldownTimer;
	float rollCooldown;

	bool isAttacking;
	float currentAttackAngle;
	Player(float startX, float startY);
	std::vector<int> enemiesHitInThisAttack;

	sf::Clock attackTimer;
	sf::Clock cooldownTimer;
	float attackDuration;
	float attackCooldown;

	int keys;

	void update(sf::RenderWindow& window, sf::View& view);
	void draw(sf::RenderWindow& window);
};

