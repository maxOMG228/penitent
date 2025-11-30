#include "Player.h"
#include <cmath>

const float PI = 3.14159265f;

Player::Player(float startX, float startY) {
	hp = 100;
	maxHp = 100;
	speed = 5.f;

	if (!texture.loadFromFile("textures\\rogue.png")) {
		// Якщо помилка, спрайт буде просто білим
	}

	sprite.setTexture(texture);
	sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
	sprite.setScale(0.5f, 0.5f);

	hitbox.setSize(sf::Vector2f(50.f, 80.f));
	hitbox.setOrigin(hitbox.getSize().x / 2.f, hitbox.getSize().y / 2.f);
	hitbox.setPosition(startX, startY);
	hitbox.setFillColor(sf::Color::Transparent);
	hitbox.setOutlineColor(sf::Color::Green);

	swordHitbox.setSize(sf::Vector2f(20.f, 40.f));
	swordHitbox.setOrigin(swordHitbox.getSize().x / 2.f, swordHitbox.getSize().y);
	swordHitbox.setFillColor(sf::Color::Red);
	swordHitbox.setPosition(-100.f, -100.f); // Start off-screen

	isAttacking = false;
	attackDuration = 0.2f;
	attackCooldown = 0.5f;
	currentAttackAngle = 0.f;

	sprite.setPosition(hitbox.getPosition());


}

void Player::update(sf::RenderWindow& window) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		hitbox.move(0, -speed);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		hitbox.move(0, +speed);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		hitbox.move(-speed, 0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		hitbox.move(+speed, 0);
	}

	sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
	sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);

	sf::Vector2f playerPos = hitbox.getPosition();
	sprite.setPosition(hitbox.getPosition());


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldownTimer.getElapsedTime().asSeconds() > attackCooldown) {
		isAttacking = true;
		attackTimer.restart();
		cooldownTimer.restart();
		enemiesHitInThisAttack.clear();

		float dx = mousePos.x - playerPos.x;
		float dy = mousePos.y - playerPos.y;
		currentAttackAngle = std::atan2(dy, dx);
	}

	if (isAttacking) {
		float offset = 60.f;
		float swordX = playerPos.x + std::cos(currentAttackAngle) * offset;
		float swordY = playerPos.y + std::sin(currentAttackAngle) * offset;

		swordHitbox.setPosition(swordX, swordY);
		swordHitbox.setRotation((currentAttackAngle * 180.f / PI) + 90.f);

		if (attackTimer.getElapsedTime().asSeconds() >= attackDuration) {
			isAttacking = false;
			swordHitbox.setPosition(-100.f, -100.f); // Move sword hitbox off-screen
		}
	}

	if (mousePos.x < hitbox.getPosition().x) {
		sprite.setScale(-0.5f, 0.5f);
	}
	else {
		sprite.setScale(0.5f, 0.5f);
	}
}

void Player::draw(sf::RenderWindow& window) {
	window.draw(sprite);
	// window.draw(hitbox);
	if (isAttacking) {
		window.draw(swordHitbox);
	}
}