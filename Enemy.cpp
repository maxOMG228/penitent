#include "Enemy.h"
#include <cmath>

Enemy::Enemy(float x, float y, int room, EnemyType t) {
	startX = x;
	startY = y;
	roomID = room;
	type = t;
	isEnemyAlive = true;
	Shape.setPosition(x, y);

	if (type == BaseEnemy) {
		hp = 10;
		Shape.setSize(sf::Vector2f(80.f, 80.f));
		Shape.setFillColor(sf::Color::Blue);
		Shape.setOrigin(40.f, 40.f);
	}
}

void Enemy::update(sf::Vector2f playerPos) {
	if (!isEnemyAlive) return;

	float speed = 0.f;
	if (type == BaseEnemy) speed = 3.f;
		
		sf::Vector2f myPos = Shape.getPosition();
		float dx = playerPos.x - myPos.x;
		float dy = playerPos.y - myPos.y;
		float dist = std::sqrt(dx * dx + dy * dy);

		if (dist > 10.f) {
			Shape.move((dx / dist) * speed, (dy / dist) * speed);
		}
	
}

