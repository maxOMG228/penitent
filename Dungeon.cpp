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

    for (int i = 0; i < 4; i++) {
        if (i == 0 || i == 2) doorTriggers[i].setSize(sf::Vector2f(100.f, 20.f));
        else doorTriggers[i].setSize(sf::Vector2f(20.f, 100.f));

        doorTriggers[i].setOrigin(doorTriggers[i].getSize().x / 2.f, doorTriggers[i].getSize().y / 2.f);
        doorTriggers[i].setPosition(-500.f, -500.f);
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
                newRoom->generateBackground(tileset, 800, 600);

                Chest* chest = new Chest(400.f, 300.f);
                chest->setLoot(getRandomArtifact(400.f, 300.f));
                newRoom->chests.push_back(chest);
            }
            else { 
                newRoom->type = Normal;

                float randomW = 1000.f + (rand() % 11) * 100.f;
                float randomH = 1000.f + (rand() % 11) * 100.f;

                newRoom->generateBackground(tileset, randomW, randomH);

                int enemyCount = rand() % 3 + 1;
                for (int e = 0; e < enemyCount; e++) {
                    EnemyType randomType = (rand() % 2 == 0) ? BaseEnemy : ArcherEnemy;
                    newRoom->addEnemy(100.f + rand() % ((int)randomW - 200), 100.f + rand() % ((int)randomH - 200), randomType);
                }
            }

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

    float w = activeRoom->roomWidth;
    float h = activeRoom->roomHeight;

    doorTriggers[0].setPosition(w / 2.f, 10.f);          // Top
    doorTriggers[1].setPosition(w - 10.f, h / 2.f);      // Right 
    doorTriggers[2].setPosition(w / 2.f, h - 10.f);      // Bottom 
    doorTriggers[3].setPosition(10.f, h / 2.f);          // Left

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
                        int nextRoomID = activeRoom->nextRoomIndex[i];
                        int entryDoorSide = (i + 2) % 4;

                        currentRoomIndex = activeRoom->nextRoomIndex[i];
                        Room* nextRoom = rooms[currentRoomIndex];

                        //new room size
                        float nextW = nextRoom->roomWidth;
                        float nextH = nextRoom->roomHeight;

                        sf::Vector2f spawnPos;
                        float offset = 150.f;

                        switch (entryDoorSide) {
                        case 0:
                            spawnPos.x = nextW / 2.f;
                            spawnPos.y = 10.f + offset;
                            break;
                        case 1:
                            spawnPos.x = nextW - 10.f - offset;
                            spawnPos.y = nextH / 2.f;
                            break;
                        case 2:
                            spawnPos.x = nextW / 2.f;
                            spawnPos.y = nextH - 10.f - offset;
                            break;
                        case 3:
                            spawnPos.x = 10.f + offset;
                            spawnPos.y = nextH / 2.f;
                            break;
                        }

                        player.hitbox.setPosition(spawnPos);

                        return;
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

    float currentRoomW = activeRoom->roomWidth;
    float currentRoomH = activeRoom->roomHeight;

    // Використовуємо size_t, щоб уникнути попереджень про типи даних

    for (size_t i = 0; i < activeRoom->enemies.size(); i++) {
        Enemy& enemy = activeRoom->enemies[i];

        if (enemy.isEnemyAlive) {
            enemy.update(player.hitbox.getPosition(), currentRoomW, currentRoomH);

            // Ворог атакує гравця
            if (enemy.type == BaseEnemy) {
            if (player.hitbox.getGlobalBounds().intersects(enemy.Shape.getGlobalBounds())) {
                if (damageClock.getElapsedTime().asSeconds() >= iFrameCooldown && !player.isRolling) {
                    player.hp -= 10;
                    damageClock.restart();
                }
            }
            }
            // bullet damage
            for (size_t b = 0; b < enemy.bullets.size(); ) {
                if (player.hitbox.getGlobalBounds().intersects(enemy.bullets[b].shape.getGlobalBounds())) {
                    if (!player.isRolling) {
                    player.hp -= 10;
                    enemy.bullets.erase(enemy.bullets.begin() + b);
                    }
                    else {
                        b++;
                    }
                }
                else {
                    b++;
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

    for (auto& enemy : activeRoom->enemies) {
        if (enemy.isEnemyAlive) {
            enemy.sprite.setTexture(enemy.texture);
            window.draw(enemy.sprite);

            for (auto& bullet : enemy.bullets) {
                if (Bullet::isTextureLoaded) {
                    window.draw(bullet.sprite);
                }
                else {
                    window.draw(bullet.shape);
                }
            }
        }
    }
}