#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "Enemy.h"
#include "Player.h"

using namespace sf;

int main()
{
    const int windowWidth = 800;
    const int windowHeight = 600;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Panties Game");
    window.setFramerateLimit(60);

    // --- GAME OVER ---
    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
    }

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER\nPress R");
    gameOverText.setCharacterSize(64);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setStyle(Text::Bold);
    // Центруємо текст
    FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(windowWidth / 2.0f, windowHeight / 2.0f);

    bool isDead = false;

    // --- ГРАВЕЦЬ ---
    Player player(400.f, 300.f);

    // --- ФОН ---
    Texture bgTexture;
    if (!bgTexture.loadFromFile("textures\\background.jpg")) {
        // Якщо фону немає, код не впаде, але екран буде чорним
        std::cout << "Background not found!" << std::endl;
    }
    Sprite background(bgTexture);

    // Масштаб фону
    if (bgTexture.getSize().x > 0) { // Перевірка, щоб не ділити на 0
        float scaleX = (float)windowWidth / bgTexture.getSize().x;
        float scaleY = (float)windowHeight / bgTexture.getSize().y;
        background.setScale(scaleX, scaleY);
    }

    // --- HP BAR ---
    RectangleShape hpBarBack(Vector2f(100.f, 10.f));
    hpBarBack.setFillColor(Color(50, 50, 50));
    hpBarBack.setOutlineColor(Color::Black);
    hpBarBack.setOutlineThickness(1.f);
    hpBarBack.setPosition(10.f, 10.f);

    RectangleShape hpBarFront(Vector2f(100.f, 10.f));
    hpBarFront.setFillColor(Color::Green);
    hpBarFront.setPosition(10.f, 10.f);

    // --- ВОРОГИ ---
    std::vector<Enemy> enemies;
    enemies.push_back(Enemy(200.f, 200.f, 1, BaseEnemy));

    int currentRoom = 1;

    // --- ДВЕРІ ---
    RectangleShape doorTrigger(Vector2f(20.f, 100.f));
    doorTrigger.setOrigin(10.f, 50.f);
    doorTrigger.setFillColor(Color::Green);

    Vector2f doorPositions[4] = {
        Vector2f(windowWidth / 2.f, 10.f),            // Top
        Vector2f(windowWidth - 10.f, windowHeight / 2.f), // Right (1)
        Vector2f(windowWidth / 2.f, windowHeight - 10.f), // Bottom
        Vector2f(10.f, windowHeight / 2.f)            // Left (3)
    };

    doorTrigger.setPosition(doorPositions[1]);

    // Таймер шкоди
    sf::Clock damageClock;
    float iFrameCooldown = 0.5f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // RESTART
            if (isDead && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                isDead = false;
                player.hp = player.maxHp;
                currentRoom = 1;

                // ! ВИПРАВЛЕНО: Встановлюємо позицію, а не колір
                player.hitbox.setPosition(windowWidth / 2.f, windowHeight / 2.f);
                player.hitbox.setFillColor(Color(255, 0, 0, 100)); // Повертаємо колір гравцю

                enemies.clear();
                enemies.push_back(Enemy(200.f, 200.f, 1, BaseEnemy));

                doorTrigger.setPosition(doorPositions[1]);
                background.setColor(Color::White);
            }
        }

        if (player.hp <= 0) isDead = true;

        if (!isDead)
        {
            // 1. Оновлення гравця
            player.update(window);

            // 2. Логіка кімнат
            if (player.hitbox.getGlobalBounds().intersects(doorTrigger.getGlobalBounds()))
            {
                if (currentRoom == 1) {
                    currentRoom = 2;
                    player.hitbox.setPosition(doorPositions[3].x + 100.f, doorPositions[3].y);
                    doorTrigger.setPosition(doorPositions[3]);
                    background.setColor(Color(200, 200, 255));
                }
                else if (currentRoom == 2) {
                    currentRoom = 1;
                    player.hitbox.setPosition(doorPositions[1].x - 100.f, doorPositions[1].y);
                    doorTrigger.setPosition(doorPositions[1]);
                    background.setColor(Color::White);

                    if (!enemies.empty()) enemies[0].Shape.setPosition(enemies[0].startX, enemies[0].startY);
                }
            }

            // 3. Обмеження екрану
            Vector2f pos = player.hitbox.getPosition();
            if (pos.x < 0) player.hitbox.setPosition(0, pos.y);
            if (pos.x > windowWidth) player.hitbox.setPosition(windowWidth, pos.y);
            if (pos.y < 0) player.hitbox.setPosition(pos.x, 0);
            if (pos.y > windowHeight) player.hitbox.setPosition(pos.x, windowHeight);

            // 4. Логіка ворогів
            for (size_t i = 0; i < enemies.size(); i++)
            {
                if (enemies[i].roomID == currentRoom && enemies[i].isEnemyAlive)
                {
                    // Рух ворога
                    enemies[i].update(player.hitbox.getPosition());

                    // Ворог б'є гравця
                    if (player.hitbox.getGlobalBounds().intersects(enemies[i].Shape.getGlobalBounds())) {
                        if (damageClock.getElapsedTime().asSeconds() >= iFrameCooldown) {
                            player.hp -= 10;
                            damageClock.restart();
                        }
                    }

                    // Гравець б'є ворога
                    if (player.isAttacking && player.swordHitbox.getGlobalBounds().intersects(enemies[i].Shape.getGlobalBounds()))
                    {
                        bool alreadyHit = false;
                        for (int id : player.enemiesHitInThisAttack) {
                            if (id == i) { alreadyHit = true; break; }
                        }

                        if (!alreadyHit) {
                            player.enemiesHitInThisAttack.push_back(i);
                            enemies[i].hp--;

                            // Відштовхування
                            float knock = 20.f;
                            enemies[i].Shape.move(std::cos(player.currentAttackAngle) * knock, std::sin(player.currentAttackAngle) * knock);

                            if (enemies[i].hp <= 0) enemies[i].isEnemyAlive = false;
                        }
                    }
                }
            }

            // Оновлення смужки HP
            // ! ВИПРАВЛЕНО: Додано (float), щоб ділення не давало 0
            float hpPercent = (float)player.hp / (float)player.maxHp;
            if (hpPercent < 0) hpPercent = 0;
            hpBarFront.setSize(Vector2f(100.f * hpPercent, 10.f));

            if (hpPercent > 0.5f) hpBarFront.setFillColor(Color::Green);
            else if (hpPercent > 0.2f) hpBarFront.setFillColor(Color::Yellow);
            else hpBarFront.setFillColor(Color::Red);
        }

        // --- МАЛЮВАННЯ ---
        window.clear();

        window.draw(background);
        window.draw(doorTrigger);
        window.draw(hpBarBack);
        window.draw(hpBarFront);

        for (auto& en : enemies) {
            if (en.roomID == currentRoom && en.isEnemyAlive) {
                window.draw(en.Shape);
            }
        }

        player.draw(window);

        if (isDead) {
            window.draw(gameOverText);
            player.hitbox.setFillColor(Color(100, 100, 100)); // Сірий колір при смерті
        }

        window.display();
    }

    return 0;
}