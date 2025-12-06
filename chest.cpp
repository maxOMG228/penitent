#include "Chest.h"
#include <iostream>

Chest::Chest(float x, float y) {
	isOpen = false;
	loot = nullptr;

	if (!texture.loadFromFile("textures\\chest\\chest_base.png")) {
	}

	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getGlobalBounds().width / 2.f, sprite.getGlobalBounds().height / 2.f);
	sprite.setPosition(x, y);
	sprite.setScale(4.f, 4.f);
}

Chest::~Chest() {
	if (loot != nullptr) {
		delete loot;
		loot = nullptr;
	}
}

void Chest::setLoot(Artifact* item) {
	loot = item;

	if (loot != nullptr) {
		loot->sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + 100.f);
	}
}

Artifact* Chest::open() {
	if (!isOpen) {
		isOpen = true;

		if (!texture.loadFromFile("textures/2D Pixel Dungeon Asset Pack/items and trap_animation/chest/chest_open_3.png")) {
			std::cout << "Error loading open chest texture" << std::endl;
		}

		sprite.setTexture(texture);
		Artifact* temp = loot;
		loot = nullptr;  // remove loot from chest after opening
		return temp;
	}
	return nullptr;
}