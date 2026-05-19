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
	Shape.setOutlineColor(sf::Color::Yellow);   // Жовта рамка
	Shape.setOutlineThickness(2.f);
	isJumping = false;
	jumpTimer.restart();
	shootTimer.restart();
	attackTimer.restart();
	windUpTimer.restart();
	isPreparingAttack = false;
	readyToHit = false;
	windUpDuration = 0.5f;
	shootInterval = 0.f;

	float barWidth = 40.f;
	float barHeight = 6.f;
	hpBarBack.setSize(sf::Vector2f(barWidth, barHeight));
	hpBarBack.setFillColor(sf::Color(100, 0, 0));
	hpBarBack.setOrigin(barWidth / 2.f, barHeight / 2.f);

	hpBarFront.setSize(sf::Vector2f(barWidth, barHeight));
	hpBarFront.setFillColor(sf::Color::Green);
	hpBarFront.setOrigin(barWidth / 2.f, barHeight / 2.f);

	if (type == MelleSkeleton) {
		if (!texture.loadFromFile("textures/Enemy_Animations_Set/enemies-skeleton1_idle.png")) {
			std::cout << "Error loading texture" << std::endl;
		}
		if (!attackTexture.loadFromFile("textures/Enemy_Animations_Set/enemies-skeleton1_attack.png")) {
			std::cout << "Error loading attack texture" << std::endl;
		}
		if (!walkTexture.loadFromFile("textures/Enemy_Animations_Set/enemies-skeleton1_movement.png")) {
			std::cout << "Error loading walk texture" << std::endl;
		}
		frameWidth = 23;
		frameHeight = 29;
		attackFrameCount = 9;
		attackFrameWidth = 32;
		attackFrameHeight = attackTexture.getSize().y;

		walkFrameCount = 10;
		int walkFrameWidth = 32;

		hp = 20;
		maxHp = 20;
		damage = 15;
		attackRange = 75.f;
		attackCooldown = 1.5f;
		windUpDuration = 0.6f;

		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
		sprite.setScale(4.f, 4.f);
	}

	if (type == BaseEnemy) {
		if (!texture.loadFromFile("textures/enemyes/Base_Slime_anim.png")) {

		}
		sprite.setTexture(texture);
		frameWidth = 23;
		frameHeight = 29;
		hp = 10;
		maxHp = 10;
		shootInterval = 0.f;
	}

	else if (type == ArcherEnemy) {
		if (!texture.loadFromFile("textures/enemyes/Skeleton_Archer.png")) {
		}
		hp = 5;
		maxHp = 5;
		shootInterval = 2.f;
		sprite.setTexture(texture);
		frameWidth = texture.getSize().x;
		frameHeight = texture.getSize().y;

		sprite.setScale(2.5f, 2.5f);
	}

	else if (type == BossEnemy) {
		if (!texture.loadFromFile("textures/boss_textures/idle.png")) {
			std::cout << "Error loading boss texture\n";
		}
		if (!bossAttackTexture.loadFromFile("textures/boss_textures/attacking.png")) {
			std::cout << "Error loading boss attack texture\n";
		}
		sprite.setTexture(texture);
		frameWidth = texture.getSize().x / 4;
		frameHeight = texture.getSize().y / 2;

		hp = 50;
		maxHp = 50;
		damage = 25;

		sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
		sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
		sprite.setScale(4.f, 4.f);

		Shape.setSize(sf::Vector2f(60.f, 100.f));
		Shape.setOrigin(30.f, 50.f);

		bossWeaponHitbox.setSize(sf::Vector2f(100.f, 150.f));
		bossWeaponHitbox.setOrigin(50.f, 75.f);
		bossWeaponHitbox.setFillColor(sf::Color(255, 0, 0, 100));
		bossWeaponHitbox.setOutlineThickness(2.f);
		bossWeaponHitbox.setOutlineColor(sf::Color::Red);
		bossWeaponHitbox.setPosition(-1000.f, -1000.f);

		bossState = BossChase;
		hasAttackedSinceLastDodge = false;
		bossHitsTaken = 0;
		bossDamageTaken = 0;
		stunHitsTaken = 0;
		lungePhase = 1;
		bossIdleTimer.restart();
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
		sprite.setTexture(texture);
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
		sprite.setTexture(texture);
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
			sprite.setTexture(attackTexture);

			float timePassed = windUpTimer.getElapsedTime().asSeconds();

			float progress = timePassed / windUpDuration;
			int currentFrame = static_cast<int>(progress * attackFrameCount);

			if (currentFrame >= attackFrameCount) currentFrame = attackFrameCount - 1;

			int left = currentFrame * attackFrameWidth;
			sprite.setTextureRect(sf::IntRect(left, 0, attackFrameWidth, attackFrameHeight));

			sprite.setOrigin(attackFrameWidth / 2.f, attackFrameHeight / 2.f);

			if (timePassed >= windUpDuration) {
				isPreparingAttack = false;
				readyToHit = true;
				attackTimer.restart();
			}
		}
		else {
			sprite.setTexture(texture);
			sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);

			sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

			if (dist <= attackRange && attackTimer.getElapsedTime().asSeconds() >= attackCooldown) {
				isPreparingAttack = true;
				windUpTimer.restart();
			}

			if (dist > attackRange) {
				sprite.setTexture(walkTexture);
				float time = attackTimer.getElapsedTime().asSeconds();
				float animSpeed = 0.1f;
				int currentWalkFrame = (int)(time / animSpeed) % walkFrameCount;
				int walkW = 32;
				int walkH = 32;
				sprite.setTextureRect(sf::IntRect(currentWalkFrame * walkW, 0, walkW, walkH));
				sprite.setOrigin(walkW / 2.f, walkH / 2.f);
				float moveX = (dx / dist) * speed;
				float moveY = (dy / dist) * speed;

				Shape.move(moveX, moveY);

				if (dx < 0) sprite.setScale(-4.f, 4.f);
				else sprite.setScale(4.f, 4.f);
			}
		}
	}
	else if (type == BossEnemy) {
		float speed = 2.0f;
		float dashSpeed = 25.0f;
		float orbitDistance = 250.f;

		if (bossState != BossStunned) {
			sprite.setColor(sf::Color::White);
		}
		else {
			sprite.setColor(sf::Color(100, 100, 100));
		}

		for (int i = 0; i < afterimages.size(); i++) {
			afterimages[i].alpha -= 10.f;
			if (afterimages[i].alpha <= 0) {
				afterimages.erase(afterimages.begin() + i);
				i--;
			}
			else {
				afterimages[i].sprite.setColor(sf::Color(150, 0, 255, (sf::Uint8)afterimages[i].alpha));
			}
		}

		if (bossState == BossOrbiting || bossState == BossLunging) {
			bossHitsTaken = 0;
			bossDamageTaken = 0;
		}

		bool insideDodgeCooldown = windUpTimer.getElapsedTime().asSeconds() >= 7.0f;
		if ((hasAttackedSinceLastDodge || insideDodgeCooldown) && (bossDamageTaken >= 3 || bossHitsTaken >= 2) && bossState != BossEvasiveDash) {
			bossState = BossEvasiveDash;
			stateTimer.restart();
			windUpTimer.restart();
			bossHitsTaken = 0;
			bossDamageTaken = 0;
			hasAttackedSinceLastDodge = false;
			bossWeaponHitbox.setPosition(-1000.f, -1000.f);
			if (dist > 0) {
				jumpDirection = sf::Vector2f(-dx / dist, -dy / dist);
			}
		}

		if (bossState == BossChase) {
			Shape.move((dx / dist) * speed, (dy / dist) * speed);

			float time = attackTimer.getElapsedTime().asSeconds();
			int currentFrame = (int)(time / 0.15f) % 8;
			sprite.setTexture(texture);
			sprite.setTextureRect(sf::IntRect((currentFrame % 4) * frameWidth, (currentFrame / 4) * frameHeight, frameWidth, frameHeight));

			if (dist < 80.f && bossIdleTimer.getElapsedTime().asSeconds() > 0.5f) {
				bossState = BossBasicAttack;
				stateTimer.restart();
			}

			else if (dist > 100.f && bossIdleTimer.getElapsedTime().asSeconds() > 0.45f && shootTimer.getElapsedTime().asSeconds() > 10.0f) {
				int randomChance = rand() % 120;
				if (randomChance < 2) {
					bossState = BossOrbiting;
					stateTimer.restart();
					shootTimer.restart();
					orbitAngle = std::atan2(Shape.getPosition().y - playerPos.y, Shape.getPosition().x - playerPos.x);
					lungePhase = 1;
				}
			}
		}

		else if (bossState == BossBasicAttack) {
			sprite.setTexture(bossAttackTexture);
			int attackFrameW = bossAttackTexture.getSize().x / 6;
			int attackFrameH = bossAttackTexture.getSize().y / 3;

			float timePassed = stateTimer.getElapsedTime().asSeconds();
			float windUpDuration = 0.5f;
			float attackDuration = 0.3f;

			if (timePassed < windUpDuration) {
				int currentFrame = 1;
				sprite.setTextureRect(sf::IntRect(currentFrame * attackFrameW, 0, attackFrameW, attackFrameH));
				bossWeaponHitbox.setPosition(-1000.f, -1000.f);
			}
			else {
				float activeAttackTime = timePassed - windUpDuration;

				int currentFrame = 2 + (int)((activeAttackTime / attackDuration) * 4);
				if (currentFrame > 5) currentFrame = 5;

				sprite.setTextureRect(sf::IntRect(currentFrame * attackFrameW, 0, attackFrameW, attackFrameH));

				float hitOffsetX = (sprite.getScale().x > 0) ? 80.f : -80.f;
				bossWeaponHitbox.setPosition(Shape.getPosition().x + hitOffsetX, Shape.getPosition().y);

				if (activeAttackTime >= attackDuration) {
					hasAttackedSinceLastDodge = true;
					bossState = BossChase;
					bossIdleTimer.restart();
					bossWeaponHitbox.setPosition(-1000.f, -1000.f);
				}
			}
		}

		else if (bossState == BossOrbiting) {
			sprite.setTexture(texture);
			orbitAngle += 0.06f;

			float targetX = playerPos.x + std::cos(orbitAngle) * orbitDistance;
			float targetY = playerPos.y + std::sin(orbitAngle) * orbitDistance;

			float ox = targetX - myPos.x;
			float oy = targetY - myPos.y;
			float odist = std::sqrt(ox * ox + oy * oy);

			float orbitCatchUpSpeed = speed * 10.0f;
			if (odist > orbitCatchUpSpeed) {
				Shape.move((ox / odist) * orbitCatchUpSpeed, (oy / odist) * orbitCatchUpSpeed);
			}
			else {
				Shape.setPosition(targetX, targetY);
			}

			bool finishOrbit = false;
			if (lungePhase == 1 && stateTimer.getElapsedTime().asSeconds() > 1.5f) finishOrbit = true;
			if (lungePhase == 2 && std::abs(orbitAngle - orbitTargetAngle) < 0.1f) finishOrbit = true;
			if (lungePhase == 2 && stateTimer.getElapsedTime().asSeconds() > 2.0f) finishOrbit = true;

			if (finishOrbit) {
				bossState = BossLunging;
				stateTimer.restart();
			}
		}
		else if (bossState == BossLunging) {
			sprite.setTexture(bossAttackTexture);
			int attackFrameW = bossAttackTexture.getSize().x / 6;
			int attackFrameH = bossAttackTexture.getSize().y / 3;

			float timePassed = stateTimer.getElapsedTime().asSeconds();

			// Якщо це перша атака, 0.5 секунди замаху
			float specialWindUp = (lungePhase == 1) ? 0.5f : 0.5f;
			float dashDuration = 0.33f;

			if (timePassed < specialWindUp) {
				int currentFrame = 1;
				sprite.setTextureRect(sf::IntRect(currentFrame * attackFrameW, 0, attackFrameW, attackFrameH));

				float lx = playerPos.x - Shape.getPosition().x;
				float ly = playerPos.y - Shape.getPosition().y;
				float ldist = std::sqrt(lx * lx + ly * ly);
				if (ldist > 0) {
					jumpDirection = sf::Vector2f(lx / ldist, ly / ldist);
				}
			}
			else {
				float activeDashTime = timePassed - specialWindUp;

				sprite.setTextureRect(sf::IntRect(5 * attackFrameW, 0, attackFrameW, attackFrameH));
				Shape.move(jumpDirection * dashSpeed);

				if (afterimageTimer.getElapsedTime().asSeconds() > 0.05f) {
					Afterimage shadow;
					shadow.sprite = sprite;
					shadow.alpha = 255.f;
					afterimages.push_back(shadow);
					afterimageTimer.restart();
				}

				if (activeDashTime > dashDuration) {
					hasAttackedSinceLastDodge = true;

					if (hp <= maxHp / 2 && lungePhase == 1) {
						lungePhase = 2;
						bossState = BossOrbiting;
						stateTimer.restart();
						orbitAngle = std::atan2(Shape.getPosition().y - playerPos.y, Shape.getPosition().x - playerPos.x);
						float randomAddAngle = (90.f + (rand() % 180)) * 3.1415f / 180.f;
						orbitTargetAngle = orbitAngle + randomAddAngle;
					}
					else {
						bossState = BossStunned;
						stunHitsTaken = 0;
						stateTimer.restart();
					}
				}
			}
		}
				
		else if (bossState == BossStunned) {

			if (stateTimer.getElapsedTime().asSeconds() > 3.0f || stunHitsTaken >= 2) {
				bossState = BossChase;
				sprite.setColor(sf::Color::White);
				bossIdleTimer.restart();
			}
		}
		else if (bossState == BossEvasiveDash) {
			sprite.setTexture(texture);

			float time = attackTimer.getElapsedTime().asSeconds();
			int currentFrame = (int)(time / 0.05f) % 8;
			sprite.setTextureRect(sf::IntRect((currentFrame % 4) * frameWidth, (currentFrame / 4) * frameHeight, frameWidth, frameHeight));

			Shape.move(jumpDirection * dashSpeed);

			if (afterimageTimer.getElapsedTime().asSeconds() > 0.05f) {
				Afterimage shadow;
				shadow.sprite = sprite;
				shadow.alpha = 255.f;
				afterimages.push_back(shadow);
				afterimageTimer.restart();
			}

			if (stateTimer.getElapsedTime().asSeconds() > 0.25f) {
				bossState = BossChase;
				bossIdleTimer.restart();
			}
		}

		if (bossState != BossBasicAttack && bossState != BossLunging) {
			if (dx < 0) sprite.setScale(-4.f, 4.f);
			else sprite.setScale(4.f, 4.f);
		}
		else if (bossState == BossLunging) {
			float specialWindUp = (lungePhase == 1) ? 0.5f : 0.5f;
			if (stateTimer.getElapsedTime().asSeconds() < specialWindUp) {
				if (dx < 0) sprite.setScale(-4.f, 4.f);
				else sprite.setScale(4.f, 4.f);
			}
			else {
				if (jumpDirection.x < 0) sprite.setScale(-4.f, 4.f);
				else sprite.setScale(4.f, 4.f);
			}
		}
	}
	



	sf::Vector2f pos = Shape.getPosition();

	if (pos.x < 20.f) pos.x = 20.f;
	if (pos.x > roomW - 20.f) pos.x = roomW - 20.f;

	if (pos.y < 20.f) pos.y = 20.f;
	if (pos.y > roomH - 20.f) pos.y = roomH - 20.f;

	sprite.setPosition(pos);
	Shape.setPosition(pos);

	float yOffset = 40.f;
	hpBarBack.setPosition(pos.x, pos.y - yOffset);
	hpBarFront.setPosition(pos.x, pos.y - yOffset);

	if (hp < 0) hp = 0;
	float hpPercent = static_cast<float>(hp) / static_cast<float>(maxHp);

	float barWidth = 40.f;
	hpBarFront.setSize(sf::Vector2f(barWidth * hpPercent, 6.f));


}

