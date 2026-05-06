#include "Artifact.h"
#include "Player.h"
#include <iostream>

Key::Key(float x, float y) : Artifact(x, y) {
	if (!texture.loadFromFile("textures/items/key.png")) {
	}
	sprite.setTexture(texture);
	sprite.setPosition(x, y);
	sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
	sprite.setScale(2.f, 2.f);
}

void Key::apply(Player& player) {
	player.keys++; // +key to player
	std::cout << "Picked up a Key! Total: " << player.keys << std::endl;
}

// Boots
SpeedBoots::SpeedBoots(float x, float y) : Artifact(x, y) {
	name = "Boots of Speed";
	if (!texture.loadFromFile("textures/artifacts/SpeedBoots.png")) {
		sprite.setTextureRect(sf::IntRect(0, 0, 40, 40));
		sprite.setColor(sf::Color::Cyan);
	}
	sprite.setTexture(texture);
	sprite.setScale(2.f, 2.f);
}

void SpeedBoots::apply(Player& player) {
	player.speed += 2.f;
	std::cout << "Picked up " << name << "! Speed increased to " << player.speed << std::endl;
}

// Ring of +Damage
DamageRing::DamageRing(float x, float y) : Artifact(x, y) {
	name = "Ring of Power";
	if (!texture.loadFromFile("textures/artifacts/Damage_Ring.png")) {
		sprite.setTextureRect(sf::IntRect(0, 0, 40, 40));
		sprite.setColor(sf::Color::Cyan);
	}
	sprite.setTexture(texture);
	sprite.setScale(4.f, 4.f);
}	

void DamageRing::apply(Player& player) {
	player.damage += 1;
	std::cout << "Picked up " << name << "! Damage increased to " << player.damage << std::endl;
}

// Attack Speed Charm
AttackSpeedCharm::AttackSpeedCharm(float x, float y) : Artifact(x, y) {
	name = "Charm of Fury";
	if (!texture.loadFromFile("textures/artifacts/Attack_Speed_Charm.png")) {
		sprite.setTextureRect(sf::IntRect(0, 0, 40, 40));
		sprite.setColor(sf::Color::Cyan);
	}
	sprite.setTexture(texture);
	sprite.setScale(2.f, 2.f);
}

void AttackSpeedCharm::apply(Player& player) {
	player.attackCooldown *= 0.8f;
	if (player.attackCooldown < 0.1f) {
		player.attackCooldown = 0.1f; // Minimum cooldown
	}

	player.attackDuration *= 0.8f;
	if (player.attackDuration < 0.1f) {
		player.attackDuration = 0.1f; // Minimum cooldown
	}

	std::cout << "Picked up " << name << "! Attack cooldown decreased to " << player.attackCooldown << " seconds." << std::endl;
}