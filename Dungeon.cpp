#include "Dungeon.h"
#include "Artifact.h"
#include "Chest.h"
#include <iostream>
#include <cmath>
#include <map>

Dungeon::Dungeon(float winW, float winH) {
    currentRoomIndex = 0;
    iFrameCooldown = 0.5f;
    if (!tileset.loadFromFile("textures//2D Pixel Dungeon Asset Pack//character and tileset//Dungeon_Tileset.png")) {
        std::cout << "Tileset texture not found!" << std::endl;
	}

	loadTextures();

    doorPositions[0] = sf::Vector2f(winW / 2.f, 10.f);          // Top
    doorPositions[1] = sf::Vector2f(winW - 10.f, winH / 2.f);   // Right
    doorPositions[2] = sf::Vector2f(winW / 2.f, winH - 10.f);   // Bottom
    doorPositions[3] = sf::Vector2f(10.f, winH / 2.f);          // Left

    for (int i = 0; i < 4; i++) {
        if (i == 0 || i == 2) doorTriggers[i].setSize(sf::Vector2f(100.f, 20.f));
        else doorTriggers[i].setSize(sf::Vector2f(20.f, 100.f));

        doorTriggers[i].setOrigin(doorTriggers[i].getSize().x / 2.f, doorTriggers[i].getSize().y / 2.f);
        doorTriggers[i].setPosition(doorPositions[i]);
    }
}

void Dungeon::loadTextures() {
    bgTextures["normal"].loadFromFile("textures/background.jpg");
    bgTextures["treasure"].loadFromFile("textures/background_treasure.jpg");
    bgTextures["boss"].loadFromFile("textures/background_boss.jpg");
}

Artifact* Dungeon::getRandomArtifact(float x, float y) {
    int roll = rand() % 100;

    // 33% шанс на кожен предмет
    if (roll < 33) {
        return new SpeedBoots(x, y);
    }
    else if (roll < 66) {
        return new DamageRing(x, y);
    }
    else {
        return new AttackSpeedCharm(x, y);
    }
}

void Dungeon::generateRandomLevel(int roomCount) {
    for (auto r : rooms) delete r;
    rooms.clear();

	Room* startRoom = new Room();
	startRoom->isCleared = true;

    startRoom->generateBackground(tileset, 800, 600);
    std::map<std::pair<int, int>, int> grid;

    rooms.push_back(startRoom);
    grid[{0, 0}] = 0;

    std::vector<int> roomQueue;
    roomQueue.push_back(0);

    int roomsCreated = 1;

    while (roomsCreated < roomCount && !roomQueue.empty()) {
        int randomIdx = rand() % roomQueue.size();
        int currentRoomID = roomQueue[randomIdx];

        int currentX = 0, currentY = 0;
        for (auto const& [pos, id] : grid) {
            if (id == currentRoomID) {
                currentX = pos.first;
                currentY = pos.second;
                break;
            }
        }

        int dir = rand() % 4;
        int nextX = currentX;
        int nextY = currentY;

        if (dir == 0) nextY -= 1;
        else if (dir == 1) nextX++;
        else if (dir == 2) nextY++;
        else if (dir == 3) nextX--;

        if (grid.find({ nextX, nextY }) == grid.end()) {
            Room* newRoom = new Room();

            int roomTypeChance = rand() % 100;

            if (roomTypeChance < 20) {
				newRoom->type = Treasure;
				newRoom->isCleared = true;

                Chest* chest = new Chest(400.f, 300.f);
                chest->setLoot(getRandomArtifact(400.f, 300.f));
                newRoom->chests.push_back(chest);
            }
            else { 
                newRoom->type = Normal;
                int enemyCount = rand() % 3 + 1;
                for (int e = 0; e < enemyCount;) newRoom->addEnemy(100.f + rand() % 600, 100.f + rand() % 400, BaseEnemy), e++;
            }

			newRoom->generateBackground(tileset, 800, 600);



            rooms.push_back(newRoom);
            int newID = rooms.size() - 1;

            grid[{ nextX, nextY }] = newID;
            roomQueue.push_back(newID);
            roomsCreated++;

            rooms[currentRoomID]->nextRoomIndex[dir] = newID;
                
            int oppositeDir = (dir + 2) % 4;
            rooms[newID]->nextRoomIndex[oppositeDir] = currentRoomID;
        }
    }
}



void Dungeon::update(Player& player, float winW, float winH) {
    Room* activeRoom = rooms[currentRoomIndex];
    activeRoom->updateStatus();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        for (auto chest : activeRoom->chests) {
            if (!chest->isOpen && player.hitbox.getGlobalBounds().intersects(chest->sprite.getGlobalBounds())) {
                Artifact* droppedItem = chest->open();
                if (droppedItem != nullptr) {
                    activeRoom->artifacts.push_back(droppedItem);
                    std::cout << "Chest opened!" << std::endl;
                }
            }
        }
    }

    for (int i = 0; i < 4; i++) {

        if (activeRoom->nextRoomIndex[i] != -1) {
            if (activeRoom->isCleared) doorTriggers[i].setFillColor(sf::Color::Green);
            else doorTriggers[i].setFillColor(sf::Color::Red);

            if (player.hitbox.getGlobalBounds().intersects(doorTriggers[i].getGlobalBounds())) {
                if (activeRoom->isCleared) {
                    currentRoomIndex = activeRoom->nextRoomIndex[i];

                    int oppositeSide = (i + 2) % 4;
                    sf::Vector2f spawnPos = doorPositions[oppositeSide];

                    if (oppositeSide == 0) spawnPos.y += 100;
                    if (oppositeSide == 1) spawnPos.x -= 100;
                    if (oppositeSide == 2) spawnPos.y -= 100;
                    if (oppositeSide == 3) spawnPos.x += 100;

                    player.hitbox.setPosition(spawnPos);
                }
            }
        }
    }

    // artifacts
    for (auto it = activeRoom->artifacts.begin(); it != activeRoom->artifacts.end();) {
        Artifact* art = *it;
        if (player.hitbox.getGlobalBounds().intersects(art->sprite.getGlobalBounds())) {
            art->apply(player);
            delete art;
            it = activeRoom->artifacts.erase(it);
        }
        else {
            it++;
        }
    }

    // Використовуємо size_t, щоб уникнути попереджень про типи даних
    for (size_t i = 0; i < activeRoom->enemies.size(); i++) {
        Enemy& enemy = activeRoom->enemies[i];

        if (enemy.isEnemyAlive) {
            enemy.update(player.hitbox.getPosition());

            // Ворог атакує гравця
            if (player.hitbox.getGlobalBounds().intersects(enemy.Shape.getGlobalBounds())) {
                if (damageClock.getElapsedTime().asSeconds() >= iFrameCooldown) {
                    player.hp -= 10;
                    damageClock.restart();
                }
            }

            // Гравець атакує ворога
            if (player.isAttacking && player.swordHitbox.getGlobalBounds().intersects(enemy.Shape.getGlobalBounds())) {
                bool alreadyHit = false;

                // Перевіряємо по ID (індексу) в межах цієї кімнати
                for (int id : player.enemiesHitInThisAttack) {
                    if (id == i) {
                        alreadyHit = true;
                        break;
                    }
                }

                if (!alreadyHit) {
                    enemy.hp -= player.damage;

                    float knock = 20.f;
                    float angle = player.currentAttackAngle;
                    enemy.Shape.move(std::cos(angle) * knock, std::sin(angle) * knock);

                    if (enemy.hp <= 0) enemy.isEnemyAlive = false;

                    player.enemiesHitInThisAttack.push_back(i);
                }
            }
        }
    }
} 

void Dungeon::draw(sf::RenderWindow& window) {
    Room* activeRoom = rooms[currentRoomIndex];

    activeRoom->backgroundSprite.setTexture(activeRoom->backgroundTexture);

	window.draw(activeRoom->backgroundSprite);

    for (auto& art : activeRoom->artifacts) {
        art->draw(window);
	}

	for (auto& chest : activeRoom->chests) {
        chest->sprite.setTexture(chest->texture);

        window.draw(chest->sprite);
    }

    for (int i = 0; i < 4; i++) {
        if (activeRoom->nextRoomIndex[i] != -1) {
            window.draw(doorTriggers[i]);
        }
	}

    for (const auto& enemy : activeRoom->enemies) {
        if (enemy.isEnemyAlive) {
            window.draw(enemy.Shape);
        }
    }
}