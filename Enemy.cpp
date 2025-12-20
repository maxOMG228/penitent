#include "Enemy.h"
#include <cmath>

sf::Texture Bullet::texture;
bool Bullet::isTextureLoaded = false;

Enemy::Enemy(float x, float y, int room, EnemyType t) {
	startX = x;
	startY = y;
	roomID = room;
	type = t;
	isEnemyAlive = true;
	Shape.setPosition(x, y);
	isJumping = false;
	jumpTimer.restart();
	shootTimer.restart();
	attackTimer.restart();
	isPreparingAttack = false;
	readyToHit = false;
	windUpDuration = 0.5f;
	shootInterval = 0.f;

	if (type == MelleSkeleton) {
		if (!texture.loadFromFile("textures/enemyes/Base_Slime_anim.png")) {
		}
		frameWidth = 23;
		frameHeight = 29;

		hp = 20;
		damage = 15; 
		attackRange = 40.f;
		attackCooldown = 1.5f;
		windUpDuration = 0.6f;

		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
		sprite.setScale(2.f, 2.f);
	}

		if (type == BaseEnemy) {
			if (!texture.loadFromFile("textures/enemyes/Base_Slime_anim.png")) {

			}
			frameWidth = 23;
			frameHeight = 29;
			hp = 10;
			shootInterval = 0.f;
		}
		else if (type == ArcherEnemy) {
			if (!texture.loadFromFile("textures/enemyes/Skeleton_Archer.png")) {
			}
			hp = 5;
			shootInterval = 2.f;

			frameWidth = texture.getSize().x;
			frameHeight = texture.getSize().y;

			sprite.setScale(2.5f, 2.5f);
		}
		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

		sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
		sprite.setPosition(x, y);
		sprite.setScale(2.f, 2.f);

		Shape.setPosition(x, y);
		Shape.setSize(sf::Vector2f(40.f, 50.f));
		Shape.setOrigin(20.f, 25.f);

		Shape.setFillColor(sf::Color::Transparent);
	}


	void Enemy::update(sf::Vector2f playerPos, float roomW, float roomH) {
		if (!isEnemyAlive) return;

		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].update();

			float bx = bullets[i].shape.getPosition().x;
			float by = bullets[i].shape.getPosition().y;
			if (bx < -100 || bx > 2000 || by < -100 || by > 2000) {
				bullets.erase(bullets.begin() + i);
				i--;
			}
		}

		sf::Vector2f myPos = Shape.getPosition();
		float dx = playerPos.x - myPos.x;
		float dy = playerPos.y - myPos.y;
		float dist = std::sqrt(dx * dx + dy * dy);

		if (type == BaseEnemy) {
			float time = jumpTimer.getElapsedTime().asSeconds();
			float dirX, dirY;
			float waitTime = 1.f;
			float jumpTime = 0.5f;
			float jumpSpeed = 6.f;

			if (isJumping) {
				dirX = jumpDirection.x;
				dirY = jumpDirection.y;
			}
			else {
				dirX = playerPos.x - Shape.getPosition().x;
				dirY = playerPos.y - Shape.getPosition().y;
			}

			int row = 0;

			if (std::abs(dirX) > std::abs(dirY)) {
				if (dirX > 0) row = 2;
				else          row = 1;
			}
			else {
				if (dirY > 0) row = 0;
				else          row = 3;
			}

			int col = 0;

			if (!isJumping) {
				col = (int)(time * 2) % 2;
			}
			else {
				col = (int)(time * 10) % 3;
			}

			int left = col * frameWidth;
			int top = row * frameHeight;

			sprite.setTextureRect(sf::IntRect(left, top, frameWidth, frameHeight));


			if (!isJumping) {
				if (time > waitTime) {

					isJumping = true;
					jumpTimer.restart();
					float dx = playerPos.x - myPos.x;
					float dy = playerPos.y - myPos.y;
					float dist = std::sqrt(dx * dx + dy * dy);

					if (dist > 10.f) {
						jumpDirection = sf::Vector2f(dx / dist, dy / dist);
					}
					else {
						jumpDirection = sf::Vector2f(0, 0);
					}
				}
			}
			else {
				if (time < jumpTime) {
					Shape.move(jumpDirection * jumpSpeed);
				}
				else {
					isJumping = false;
					jumpTimer.restart();
				}
			}
		}
		else if (type == ArcherEnemy) {
			float speed = 2.5f;
			float keepDist = 300.f;

			if (dist > keepDist + 50) {
				Shape.move((dx / dist) * speed, (dy / dist) * speed);
			}
			else if (dist < keepDist - 50) {
				Shape.move(-(dx / dist) * speed, -(dy / dist) * speed);
			}
			if (shootTimer.getElapsedTime().asSeconds() > shootInterval) {
				bullets.push_back(Bullet(myPos.x, myPos.y, dx, dy));
				shootTimer.restart();
			}
			if (dx < 0) sprite.setScale(-2.5f, 2.5f);
			else sprite.setScale(2.5f, 2.5f);
		}

		else if (type == MelleSkeleton) {
			float speed = 3.5f;
			readyToHit = false;

			if (isPreparingAttack) {
				sprite.setColor(sf::Color(255, 100, 100));

				if (windUpTimer.getElapsedTime().asSeconds() >= windUpDuration) {
					isPreparingAttack = false;
					readyToHit = true;
					attackTimer.restart();

					sprite.setColor(sf::Color::White);
				}
			}
			else {
				if (dist <= attackRange && attackTimer.getElapsedTime().asSeconds() >= attackCooldown) {
					isPreparingAttack = true;
					windUpTimer.restart();
				}

				if (dist > attackRange) {
					float moveX = (dx / dist) * speed;
					float moveY = (dy / dist) * speed;
					Shape.move(moveX, moveY);

					if (dx < 0) sprite.setScale(-2.f, 2.f);
					else sprite.setScale(2.f, 2.f);
				}
				else {
					//attack animation
				}
			}
		}



		sf::Vector2f pos = Shape.getPosition();

		if (pos.x < 20.f) pos.x = 20.f;
		if (pos.x > roomW - 20.f) pos.x = roomW - 20.f;

		if (pos.y < 20.f) pos.y = 20.f;
		if (pos.y > roomH - 20.f) pos.y = roomH - 20.f;

		sprite.setPosition(pos);
	}

