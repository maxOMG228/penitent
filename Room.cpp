#include "Room.h"

Room::Room() {
	isCleared = false;
	type = Normal;

	for (int i = 0; i < 4; i++ ){
		nextRoomIndex[i] = -1;
}
}

void Room::generateBackground(sf::Texture& tileset, float w, float h) {
	roomWidth = w;
	roomHeight = h;
	
	sf::RenderTexture rt;
	if (!rt.create(static_cast<unsigned int>(w), static_cast<unsigned int>(h))) return;

	rt.clear(sf::Color::Black);

	sf::Sprite tile(tileset);

	float scale = 4.f;
	tile.setScale(scale, scale);

	int drawnSize = 16 * scale;

	for (int y = 0; y < static_cast<int>(h); y += drawnSize) {
		for (int x = 0; x < static_cast<int>(w); x += drawnSize) {

			if (this->type == Treasure) {
				tile.setTextureRect(sf::IntRect(48, 112, 16, 16));
			}
			else {
				int chance = rand() % 100;
				if (chance < 80) {
					tile.setTextureRect(sf::IntRect(48, 112, 16, 16));
				}
				else if (chance < 95) {
					tile.setTextureRect(sf::IntRect(96, 23, 16, 16));
				}
				else {
					tile.setTextureRect(sf::IntRect(31, 25, 16, 16));
				}
			}
			tile.setPosition(x, y);
			rt.draw(tile);
		}
	}
	rt.display();

	this->bakedTexture = rt.getTexture(); 
	this->backgroundSprite.setTexture(this->bakedTexture);
	this->backgroundSprite.setTextureRect(sf::IntRect(0, 0, (int)w, (int)h));
		
}

void Room::addEnemy(float x, float y, EnemyType type) {
	enemies.push_back(Enemy(x, y, 0, type));
}

void Room::updateStatus() {
	isCleared = true;
	for (const auto& enemy : enemies) {
		if (enemy.isEnemyAlive) {
			isCleared = false;
			break;
		}
	}
}