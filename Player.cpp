#include "Player.h"
#include <cmath>

const float PI = 3.14159265f;

Player::Player(float startX, float startY) {
	if (attackTexture.loadFromFile("textures/attack.png")) {
		attackSprite.setTexture(attackTexture);

		int frameW = attackTexture.getSize().x / 4;
		int frameH = attackTexture.getSize().y;
		attackSprite.setOrigin(frameW, frameH / 2.f);

		attackSprite.setScale(2.f, 2.f);
	}

	hp = 100;
	keys = 0;
	maxHp = 100;
	speed = 5.f;
	damage = 1;

	isRolling = false;
	rollSpeed = 10.f;
	rollDuration = 0.3f;
	rollCooldown = 1.0f;

	if (!texture.loadFromFile("textures\\rogue spritesheet calciumtrice.png")) {
	}

	numFrames = 10;
	timePerFrame = 0.1f;
	currentFrame = 0;
	animTimer.restart();

	frameWidth = texture.getSize().x / numFrames;
	frameHeight = texture.getSize().y / 10;

	sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

	sprite.setTexture(texture);
	sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
	sprite.setScale(2.5f, 2.5f);

	hitbox.setSize(sf::Vector2f(50.f, 80.f));
	hitbox.setOrigin(hitbox.getSize().x / 2.f, hitbox.getSize().y / 2.f);
	hitbox.setPosition(startX, startY);
	hitbox.setFillColor(sf::Color::Transparent);
	hitbox.setOutlineColor(sf::Color::Green);
	hitbox.setOutlineThickness(2.f);

	swordHitbox.setSize(sf::Vector2f(20.f, 80.f));
	swordHitbox.setOrigin(swordHitbox.getSize().x / 2.f, swordHitbox.getSize().y);
	swordHitbox.setFillColor(sf::Color::Red);
	swordHitbox.setPosition(-100.f, -100.f); // Start off-screen

	isAttacking = false;
	attackDuration = 0.5f;
	attackCooldown = 0.5f;
	currentAttackAngle = 0.f;

	sprite.setPosition(hitbox.getPosition());


}

void Player::update(sf::RenderWindow& window, sf::View& view) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isRolling && rollCooldownTimer.getElapsedTime().asSeconds() > rollCooldown) {
		float dirX = 0.f;
		float dirY = 0.f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dirX = -1.f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dirX = 1.f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dirY = -1.f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dirY = 1.f;

		if (dirX != 0 || dirY != 0) {
			isRolling = true;
			rollTimer.restart();
			rollCooldownTimer.restart();

			float length = std::sqrt(dirX * dirX + dirY * dirY);
			rollDirection = sf::Vector2f(dirX / length, dirY / length);
		}
	}

	if (isRolling) {
		hitbox.move(rollDirection * rollSpeed);

		if (rollTimer.getElapsedTime().asSeconds() > rollDuration) {
			isRolling = false;
		}
	}else{
		bool isMoving = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		hitbox.move(0, -speed);
		isMoving = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		hitbox.move(0, +speed);
		isMoving = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		hitbox.move(-speed, 0);
		isMoving = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		hitbox.move(+speed, 0);
		isMoving = true;
	}
	sprite.setPosition(hitbox.getPosition());

	if (isMoving) {
		if (animTimer.getElapsedTime().asSeconds() > timePerFrame) {
			animTimer.restart();
			currentFrame++;

			if (currentFrame >= numFrames) {
				currentFrame = 0;
			}
		}
}
	else {
		currentFrame = 0;
	}
	int animationRow = 2;

	int left = currentFrame * frameWidth;
	int top = animationRow * frameHeight;
	sprite.setTextureRect(sf::IntRect(left, top, frameWidth, frameHeight));
}

	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);

	sf::Vector2f playerPos = hitbox.getPosition();
	sprite.setPosition(hitbox.getPosition());


	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && cooldownTimer.getElapsedTime().asSeconds() > attackCooldown) {
		isAttacking = true;
		attackTimer.restart();
		cooldownTimer.restart();
		enemiesHitInThisAttack.clear();

		float dx = mousePos.x - playerPos.x;
		float dy = mousePos.y - playerPos.y;
		currentAttackAngle = std::atan2(dy, dx);
	}

	if (isAttacking) {
		float offset = 30.f;
		float swordX = playerPos.x + std::cos(currentAttackAngle) * offset;
		float swordY = playerPos.y + std::sin(currentAttackAngle) * offset;

		swordHitbox.setPosition(swordX, swordY);
		swordHitbox.setRotation((currentAttackAngle * 180.f / PI) + 90.f);
		attackSprite.setPosition(swordX, swordY);
		attackSprite.setRotation((currentAttackAngle * 180.f / PI) + 180.f);

		float timePassed = attackTimer.getElapsedTime().asSeconds();
		float progress = timePassed / attackDuration;

		int maxFrames = 4;
		int animFrame = static_cast<int>(progress * maxFrames);
		if (animFrame >= maxFrames) animFrame = maxFrames - 1;

		int frameW = attackTexture.getSize().x / 4;
		int frameH = attackTexture.getSize().y;
		attackSprite.setTextureRect(sf::IntRect(animFrame * frameW, 0, frameW, frameH));

		int charAttackFrames = 10;
		int charAnimFrame = static_cast<int>(progress * charAttackFrames);
		if (charAnimFrame >= charAttackFrames) charAnimFrame = charAttackFrames - 1;

		int attackRow = 3;

		int left = charAnimFrame * frameWidth;
		int top = attackRow * frameHeight;

		sprite.setTextureRect(sf::IntRect(left, top, frameWidth, frameHeight));

		if (timePassed >= attackDuration) {
			isAttacking = false;
			swordHitbox.setPosition(-100.f, -100.f);
		}
	}

	if (mousePos.x < hitbox.getPosition().x) {
		sprite.setScale(-2.5f, 2.5f);
	}
	else {
		sprite.setScale(2.5f, 2.5f);
	}
}

void Player::draw(sf::RenderWindow& window) {
	window.draw(sprite);
	//window.draw(hitbox);
	if (isAttacking) {
		//window.draw(swordHitbox);
		window.draw(attackSprite);
	}
}