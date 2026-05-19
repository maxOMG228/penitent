#include "Dungeon.h"
#include "Artifact.h"
#include "Chest.h"
#include <iostream>
#include <cmath>
#include <map>

Dungeon::Dungeon(float winW, float winH) {
    currentRoomIndex = 0;
    iFrameCooldown = 0.5f;

    if (!doorTextureVert.loadFromFile("textures/doors/vertical_door.png")) {}
    if (!doorTextureHoriz.loadFromFile("textures/doors/horisontal_door.png")) {}

    for (int i = 0; i < 4; i++) {
        doorCurrentFrame[i] = 0;

        if (i == 0 || i == 2) {
            doorSprites[i].setTexture(doorTextureHoriz);
        }
        else {
            doorSprites[i].setTexture(doorTextureVert);
        }
    }

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
    startRoom->gridX = 0;
    startRoom->gridY = 0;
    startRoom->depth = 0;

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

            newRoom->depth = rooms[currentRoomID]->depth + 1;

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
                    int typeRoll = rand() % 3;
                    EnemyType randomType;

                    if (typeRoll == 0) randomType = BaseEnemy;
                    else if (typeRoll == 1) randomType = ArcherEnemy;
                    else randomType = MelleSkeleton;

                    newRoom->addEnemy(100.f + rand() % ((int)randomW - 200), 100.f + rand() % ((int)randomH - 200), randomType);
                }
            }

            newRoom->gridX = nextX;
            newRoom->gridY = nextY;

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
    int maxDepth = -1;
    int bossRoomIndex = -1;

    for (int i = 0; i < rooms.size(); i++) {
        if (i == 0) continue;

        if (rooms[i]->depth > maxDepth) {
            maxDepth = rooms[i]->depth;
            bossRoomIndex = i;
        }
    }

    if (bossRoomIndex != -1) {
        rooms[bossRoomIndex]->type = BossRoom;
        float bossW = 1200.f;
        float bossH = 1200.f;
        rooms[bossRoomIndex]->generateBackground(tileset, bossW, bossH);

        rooms[bossRoomIndex]->enemies.clear();
        for (auto c : rooms[bossRoomIndex]->chests) delete c;
        rooms[bossRoomIndex]->chests.clear();

        rooms[bossRoomIndex]->addEnemy(bossW / 2.f, bossH / 2.f, BossEnemy);
    }
}



void Dungeon::update(Player& player, float winW, float winH) {
    Room* activeRoom = rooms[currentRoomIndex];
    activeRoom->updateStatus();

    activeRoom->isRevealed = true;
    for (int i = 0; i < 4; i++) {
        if (activeRoom->nextRoomIndex[i] != -1) {
            rooms[activeRoom->nextRoomIndex[i]]->isRevealed = true;
        }
    }

    if (activeRoom->isCleared && !activeRoom->rewardSpawned && activeRoom->type != Treasure) {
        if (currentRoomIndex != 0 && activeRoom->type != BossRoom) {
            int roll = rand() % 100;
            if (roll < 100) {
                float spawnX = activeRoom->roomWidth / 2.f;
                float spawnY = activeRoom->roomHeight / 2.f;

                activeRoom->artifacts.push_back(new Key(spawnX, spawnY));
                std::cout << "Loot spawned: Key!" << std::endl;
            }
        }
        activeRoom->rewardSpawned = true;
    }

    float w = activeRoom->roomWidth;
    float h = activeRoom->roomHeight;

    doorTriggers[0].setPosition(w / 2.f, 10.f);          // Top
    doorTriggers[1].setPosition(w - 10.f, h / 2.f);      // Right 
    doorTriggers[2].setPosition(w / 2.f, h - 10.f);      // Bottom 
    doorTriggers[3].setPosition(10.f, h / 2.f);          // Left

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        for (auto chest : activeRoom->chests) {
            if (!chest->isOpen && player.hitbox.getGlobalBounds().intersects(chest->sprite.getGlobalBounds())) {
                if (player.keys > 0) {
                    player.keys--;
                    Artifact* droppedItem = chest->open();
                    if (droppedItem != nullptr) {
                        activeRoom->artifacts.push_back(droppedItem);
                        std::cout << "Chest opened!" << std::endl;
                    }
                }
                else {
                    std::cout << "Locked! You need a key." << std::endl;
                }
            }
        }
    }


        for (int i = 0; i < 4; i++) {

            if (activeRoom->nextRoomIndex[i] != -1) {

                int maxFrames = (i == 0 || i == 2) ? 8 : 5;
                int frameWidth = (i == 0 || i == 2) ? doorTextureHoriz.getSize().x / maxFrames : doorTextureVert.getSize().x / maxFrames;
                int frameHeight = (i == 0 || i == 2) ? doorTextureHoriz.getSize().y : doorTextureVert.getSize().y;
                doorSprites[i].setScale(2.f, 2.f);

                if (activeRoom->isCleared) {
                    if (doorCurrentFrame[i] < maxFrames - 1) {
                        if (doorAnimTimers[i].getElapsedTime().asSeconds() > 0.1f) { // 0.1f - швидкість анімації
                            doorCurrentFrame[i]++;
                            doorAnimTimers[i].restart();
                        }
                    }
                }
                else {
                    doorCurrentFrame[i] = 0;
                }

                doorSprites[i].setTextureRect(sf::IntRect(doorCurrentFrame[i] * frameWidth, 0, frameWidth, frameHeight));
                doorSprites[i].setOrigin(frameWidth / 2.f, frameHeight / 2.f);
                doorSprites[i].setPosition(doorTriggers[i].getPosition());

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

                        for (int j = 0; j < 4; j++) {
                            doorCurrentFrame[j] = 0;
                        }

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


            for (size_t j = i + 1; j < activeRoom->enemies.size(); j++) {
                Enemy& otherEnemy = activeRoom->enemies[j];
                if (otherEnemy.isEnemyAlive) {
                    sf::FloatRect overlap;
                    if (enemy.Shape.getGlobalBounds().intersects(otherEnemy.Shape.getGlobalBounds(), overlap)) {
                        sf::Vector2f ePos = enemy.Shape.getPosition();
                        sf::Vector2f oPos = otherEnemy.Shape.getPosition();

                        if (overlap.width < overlap.height) {
                            float push = overlap.width / 2.f;
                            if (ePos.x < oPos.x) { enemy.Shape.move(-push, 0.f); otherEnemy.Shape.move(push, 0.f); }
                            else { enemy.Shape.move(push, 0.f); otherEnemy.Shape.move(-push, 0.f); }
                        }
                        else {
                            float push = overlap.height / 2.f;
                            if (ePos.y < oPos.y) { enemy.Shape.move(0.f, -push); otherEnemy.Shape.move(0.f, push); }
                            else { enemy.Shape.move(0.f, push); otherEnemy.Shape.move(0.f, -push); }
                        }
                    }
                }
            }

            if (enemy.type != BossEnemy && enemy.type != BaseEnemy) {
                sf::FloatRect pOverlap;
                if (player.hitbox.getGlobalBounds().intersects(enemy.Shape.getGlobalBounds(), pOverlap)) {
                    if (!player.isRolling) {
                        sf::Vector2f pPos = player.hitbox.getPosition();
                        sf::Vector2f ePos = enemy.Shape.getPosition();

                        if (pOverlap.width < pOverlap.height) {
                            if (pPos.x < ePos.x) player.hitbox.move(-pOverlap.width, 0.f);
                            else player.hitbox.move(pOverlap.width, 0.f);
                        }
                        else {
                            if (pPos.y < ePos.y) player.hitbox.move(0.f, -pOverlap.height);
                            else player.hitbox.move(0.f, pOverlap.height);
                        }
                    }
                }
            }

            // Ворог атакує гравця
            if (enemy.type == BaseEnemy) {
            if (player.hitbox.getGlobalBounds().intersects(enemy.Shape.getGlobalBounds())) {
                if (damageClock.getElapsedTime().asSeconds() >= iFrameCooldown && !player.isRolling) {
                    player.hp -= 10;
                    damageClock.restart();
                }
            }
            }
            else if (enemy.type == MelleSkeleton) {
                float dx = player.hitbox.getPosition().x - enemy.Shape.getPosition().x;
                float dy = player.hitbox.getPosition().y - enemy.Shape.getPosition().y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (enemy.readyToHit) {

                    float dx = player.hitbox.getPosition().x - enemy.Shape.getPosition().x;
                    float dy = player.hitbox.getPosition().y - enemy.Shape.getPosition().y;

                    if (std::abs(dx) <= enemy.attackRange + 20.f && std::abs(dy) <= enemy.attackRange + 60.f) {
                        if (!player.isRolling) {
                            player.hp -= enemy.damage;
                        }
                    }
                }
            }
            else if (enemy.type == BossEnemy) {
                sf::FloatRect overlap;
                if (player.hitbox.getGlobalBounds().intersects(enemy.Shape.getGlobalBounds(), overlap)) {
                    if (!player.isRolling && enemy.bossState != BossLunging) {
                        sf::Vector2f pPos = player.hitbox.getPosition();
                        sf::Vector2f bPos = enemy.Shape.getPosition();

                        if (overlap.width < overlap.height) {
                            if (pPos.x < bPos.x) player.hitbox.move(-overlap.width, 0.f);
                            else player.hitbox.move(overlap.width, 0.f);
                        }
                        else {
                            if (pPos.y < bPos.y) player.hitbox.move(0.f, -overlap.height);
                            else player.hitbox.move(0.f, overlap.height);
                        }
                    }
                }

                bool playerHit = false;

                if (enemy.bossState == BossBasicAttack) {
                    if (player.hitbox.getGlobalBounds().intersects(enemy.bossWeaponHitbox.getGlobalBounds())) {
                        playerHit = true;
                    }
                }

                else if (enemy.bossState == BossLunging) {
                    if (player.hitbox.getGlobalBounds().intersects(enemy.Shape.getGlobalBounds())) {
                        playerHit = true;
                    }
                }

                if (playerHit && damageClock.getElapsedTime().asSeconds() >= iFrameCooldown && !player.isRolling) {
                    player.hp -= enemy.damage;
                    damageClock.restart();
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

                    if (enemy.type == BossEnemy) {
                        enemy.bossDamageTaken += player.damage;
                        enemy.bossHitsTaken += 1;
                        if (enemy.bossState == BossStunned) {
                            enemy.stunHitsTaken += 1;
                        }
                    }

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
            window.draw(doorSprites[i]);
        }
    }

    for (auto& enemy : activeRoom->enemies) {
        if (enemy.isEnemyAlive) {
            //enemy.sprite.setTexture(enemy.texture);
            //window.draw(enemy.Shape);
            if (enemy.type == BossEnemy) {
                for (auto& shadow : enemy.afterimages) {
                    window.draw(shadow.sprite);
                }
               // if (enemy.bossState == BossBasicAttack) {
                    //window.draw(enemy.bossWeaponHitbox);
                //}
            }
            window.draw(enemy.sprite);

            if (enemy.hp > 0 && enemy.hp < enemy.maxHp && enemy.type != BossEnemy) {
                window.draw(enemy.hpBarBack);
                window.draw(enemy.hpBarFront);
            }


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
    if (activeRoom->type == BossRoom) {
        for (auto& enemy : activeRoom->enemies) {
            if (enemy.type == BossEnemy && enemy.isEnemyAlive) {

                float hpPercent = static_cast<float>(enemy.hp) / enemy.maxHp;
                if (hpPercent < 0) hpPercent = 0;

                float barWidth = 600.f;
                float barHeight = 20.f;

                float barX = 100.f;
                float barY = window.getSize().y - 50.f;

                sf::RectangleShape bossBg(sf::Vector2f(barWidth, barHeight));
                bossBg.setPosition(barX, barY);
                bossBg.setFillColor(sf::Color(50, 0, 0));
                bossBg.setOutlineThickness(2.f);
                bossBg.setOutlineColor(sf::Color::White);

                sf::RectangleShape bossFg(sf::Vector2f(barWidth * hpPercent, barHeight));
                bossFg.setPosition(barX, barY);
                bossFg.setFillColor(sf::Color::Red);


                sf::View currentView = window.getView(); 
                window.setView(window.getDefaultView()); 

                window.draw(bossBg);
                window.draw(bossFg);

                window.setView(currentView); 
            }
        }
    }
}


void Dungeon::drawMinimap(sf::RenderWindow& window) {
    float cellSize = 15.f;
    float padding = 2.f;

    float mapOriginX = window.getSize().x - 100.f;
    float mapOriginY = 100.f;
    int playerGridX = rooms[currentRoomIndex]->gridX;
    int playerGridY = rooms[currentRoomIndex]->gridY;

    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));

    for (auto& room : rooms) {
        if (!room->isRevealed) continue;

        int relX = room->gridX - playerGridX;
        int relY = room->gridY - playerGridY;

        float drawX = mapOriginX + (relX * (cellSize + padding));
        float drawY = mapOriginY + (relY * (cellSize + padding));

        cell.setPosition(drawX, drawY);

        if (room == rooms[currentRoomIndex]) {
            cell.setFillColor(sf::Color::Cyan);
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::White);
        }
        else {
            cell.setOutlineThickness(0.f);
            if (room->type == Treasure) {
                cell.setFillColor(sf::Color::Yellow);
            }
            else if (room->type == BossRoom) {
                cell.setFillColor(sf::Color::Red);
            }
            else if (room->isCleared) {
                cell.setFillColor(sf::Color(100, 255, 100));
            }
            else {
                cell.setFillColor(sf::Color(100, 100, 100));
            }
        }
        window.draw(cell);
    }
}