#pragma once
#include <SFML/Graphics.hpp>
#include "Artifact.h"

	class Chest {
	public:
		sf::Sprite sprite;
		sf::Texture texture;
		bool isOpen;

		// loot inside the chest
		Artifact* loot;

		Chest(float x, float y);
		~Chest();

		void setLoot(Artifact* item);  // set the loot inside the chest
		Artifact* open();	// Chest opening method
	};

