#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

#include "Enemy.h"
#include "Player.h"
#include "Room.h"
#include "Dungeon.h"

using namespace sf;

int main()
{
    const int windowWidth = 800;
    const int windowHeight = 600;

    sf::View camera(sf::Vector2f(0.f, 0.f), sf::Vector2f(windowWidth, windowHeight));

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Penitent Game");
    window.setFramerateLimit(60);

	srand(static_cast<unsigned int>(time(0)));

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
	// Text centering
    FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(windowWidth / 2.0f, windowHeight / 2.0f);

    bool isDead = false;
    bool isPaused = false;

    // PAUSE
    Text pauseText;
    pauseText.setFont(font);
    pauseText.setString("PAUSED\nPress ESC to Resume");
    pauseText.setCharacterSize(64);
    pauseText.setFillColor(Color::White);
    pauseText.setStyle(Text::Bold);

    FloatRect pauseRect = pauseText.getLocalBounds();
    pauseText.setOrigin(pauseRect.left + pauseRect.width / 2.0f, pauseRect.top + pauseRect.height / 2.0f);
    pauseText.setPosition(windowWidth / 2.0f, windowHeight / 2.0f);

    RectangleShape pauseOverlay(Vector2f(windowWidth, windowHeight));
    pauseOverlay.setFillColor(Color(0, 0, 0, 150));

    // --- PLAYER ---
    Player player(400.f, 300.f);

    // DUNGEON

    Dungeon dungeon(800.f, 600.f);
    dungeon.generateRandomLevel(10);

	// background

    // --- HP BAR ---
    RectangleShape hpBarBack(Vector2f(100.f, 10.f));
    hpBarBack.setFillColor(Color(50, 50, 50));
    hpBarBack.setOutlineColor(Color::Black);
    hpBarBack.setOutlineThickness(1.f);
    hpBarBack.setPosition(10.f, 10.f);

    RectangleShape hpBarFront(Vector2f(100.f, 10.f));
    hpBarFront.setFillColor(Color::Green);
    hpBarFront.setPosition(10.f, 10.f);


	// damage i-frames

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
                player.hitbox.setPosition(windowWidth / 2.f, windowHeight / 2.f);
                player.hitbox.setFillColor(Color(255, 0, 0, 100));

                dungeon.currentRoomIndex = 0;
                dungeon.generateRandomLevel(10);


            }
            // pause trigger
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (!isDead) {
                    isPaused = !isPaused;
                }
            }
        }

		

        if (player.hp <= 0) isDead = true;

        if (!isDead && !isPaused)
        {
			// player update
            player.update(window, camera);
            dungeon.update(player, windowWidth, windowHeight);

            Room* currentRoom = dungeon.rooms[dungeon.currentRoomIndex];
            float currentRoomW = dungeon.rooms[dungeon.currentRoomIndex]->roomWidth;
            float currentRoomH = dungeon.rooms[dungeon.currentRoomIndex]->roomHeight;


            float camX = player.hitbox.getPosition().x;
            float camY = player.hitbox.getPosition().y;

            if (camX <= windowWidth / 2.f) {
                camX = windowWidth / 2.f;
            }
            else if (camX > currentRoomW - windowWidth / 2.f) {
                camX = currentRoomW - windowWidth / 2.f;
            }


            if (camY < windowHeight / 2.f) {
                camY = windowHeight / 2.f;
            }
            else if (camY > currentRoomH - windowHeight / 2.f) {
                camY = currentRoomH - windowHeight / 2.f;
            }

            camera.setCenter(camX, camY);

            // movement limits
            currentRoom = dungeon.rooms[dungeon.currentRoomIndex];
            if (!dungeon.rooms.empty() && dungeon.currentRoomIndex >= 0 && dungeon.currentRoomIndex < dungeon.rooms.size())
            {
                float currentRoomW = currentRoom->roomWidth;
                float currentRoomH = currentRoom->roomHeight;

                Vector2f pos = player.hitbox.getPosition();
                if (pos.x < 25.f) player.hitbox.setPosition(25.f, pos.y);
                if (pos.x > currentRoomW - 25.f) player.hitbox.setPosition(currentRoomW - 25.f, pos.y);
                if (pos.y < 25.f) player.hitbox.setPosition(pos.x, 25.f);
                if (pos.y > currentRoomH - 25.f) player.hitbox.setPosition(pos.x, currentRoomH - 25.f);
            }
            // dinamic HP BAR
            
            float hpPercent = (float)player.hp / (float)player.maxHp;
            if (hpPercent < 0) hpPercent = 0;
            hpBarFront.setSize(Vector2f(100.f * hpPercent, 10.f));

            if (hpPercent > 0.5f) hpBarFront.setFillColor(Color::Green);
            else if (hpPercent > 0.2f) hpBarFront.setFillColor(Color::Yellow);
            else hpBarFront.setFillColor(Color::Red);
        }

		// DRAWING
        window.clear();

        window.setView(camera);

        dungeon.draw(window);
        player.draw(window);
        
        window.setView(window.getDefaultView());
        window.draw(hpBarBack);
        window.draw(hpBarFront);

        dungeon.drawMinimap(window);


        if (isDead) {
            window.draw(gameOverText);
            player.hitbox.setFillColor(Color(100, 100, 100)); // Gray color
        }

        if (isPaused) {
            window.setView(window.getDefaultView());
            window.draw(pauseOverlay);
            window.draw(pauseText);
        }

        window.display();
    }

    return 0;
}