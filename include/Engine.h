#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include "HealBonus.h"
#include "RecordManager.h"
#include "UIManager.h"
#include "AudioManager.h"
using namespace sf;

class Engine { 
private:
	int fps = 70;
	const int MainWindowHeight = 650, MainWindowWidth = 800;
	const int MaxEnemiesCount = 12;
	const int MaxHealBonusCount = 3;
	float keyDelay;
	float timePassed;
	RenderWindow MainWindow;
	Texture* MainBackgroundTexture;
	Sprite* MainBackgroundSprite;
	AudioManager* audioManager;
	std::vector<Enemy> enemies; 
	std::vector<Bullet> bullets;
	std::vector<HealBonus> healBonuses;
	Player* Hero;
	bool gamePaused;
	Vector2f bulletDirection;
	float bulletTimePassed;
	UIManager* uiManager;
	RecordManager* recordManager;
	int scores;

	void CheckCollisionBulletsEnemies(bool enemyIsDead, int i)
	{
		for (int j = bullets.size() - 1; j >= 0; j--)
		{
			if (!enemyIsDead)
			{
				if (enemies[i].sprite.getGlobalBounds().findIntersection(bullets[j].sprite.getGlobalBounds()).has_value())
				{
					enemies[i].IsHit();
					bullets.erase(bullets.begin() + j);
					if (enemies[i].getCurrentHealth() <= 0)
					{
						enemyIsDead = true;
						if (rand() % 3 == 1)
							spawnNewHealBonuses(enemies[i].x, enemies[i].y);
						scores += enemies[i].getMaxHealth();
						uiManager->setScoreText(scores);
						enemies.erase(enemies.begin() + i);
					}
				}
			}
		}

	}

	void CheckCollisionEnemiesEnemies(int i) //need fix
	{
		for (int j = i - 1; j >= 0; j--)
		{
			if (enemies[i].sprite.getGlobalBounds().findIntersection(enemies[j].sprite.getGlobalBounds()).has_value())
			{
				enemies[j].x += (rand() % 2) - 1;
				enemies[j].y += (rand() % 2) - 1;
			}
		}
	}

	void CheckCollisionEnemiesPlayer(int i) //need fix
	{
		if (enemies[i].sprite.getGlobalBounds().findIntersection(Hero->sprite.getGlobalBounds()).has_value())
		{
			Hero->IsHit();
			uiManager->setHealthText(Hero->getCurrentHealth());
		}
	}

	void CheckCollisionPlayerHealBonus(int i)
	{
		if (healBonuses[i].sprite.getGlobalBounds().findIntersection(Hero->sprite.getGlobalBounds()).has_value())
		{
			Hero->IsHeal();
			uiManager->setHealthText(Hero->getCurrentHealth());
			healBonuses.erase(healBonuses.begin() + i);
		}
	}

	void UpdateHealBonuses()
	{
		for (int i = healBonuses.size() - 1; i >= 0; i--)
		{
			CheckCollisionPlayerHealBonus(i);
		}
	}

	void UpdateEnemies(float dtAsSeconds)
	{
		spawnNewEnemies();
		Enemy::CurentPlayerPosition = Vector2f(Hero->x, Hero->y);
		bool enemyIsDead = false;
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			enemies[i].update(dtAsSeconds);
			CheckCollisionEnemiesPlayer(i);
			CheckCollisionEnemiesEnemies(i);
			CheckCollisionBulletsEnemies(enemyIsDead, i);
			enemyIsDead = false;
		}
	}

	void UpdateBullets(float dtAsSeconds)
	{
		setBulletDirection();
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			bullets[i].update(dtAsSeconds);

			Vector2f tmp = bullets[i].sprite.getPosition(); 
			if (tmp.x < 0 || tmp.x > MainWindowWidth || tmp.y < 0 || tmp.y > MainWindowHeight) 
			{
				bullets.erase(bullets.begin() + i);
			}
		}
	}

	void spawnNewEnemies()
	{
		if (enemies.size() < MaxEnemiesCount)
		{
			int randX = rand() % MainWindowWidth;
			int randY = rand() % MainWindowHeight;
			randX < MainWindowWidth / 2 ? randX -= MainWindowWidth / 2 + 30 : randX += MainWindowWidth / 2 + 30;
			randY < MainWindowHeight / 2 ? randY -= MainWindowHeight / 2 + 30 : randY += MainWindowHeight / 2 + 30;

			enemies.push_back(Enemy(randX, randY));
		}
	}

	void spawnNewHealBonuses(float x, float y)
	{
		if (healBonuses.size() < MaxHealBonusCount)
			healBonuses.push_back(HealBonus(x, y));
	}

	bool isGamePaused()
	{
		return gamePaused == false;
	}

	void restartGame(bool fromPause)
	{
		delete Hero;
		Hero = new Player(400, 200);
		delete recordManager;
		recordManager = new RecordManager(scores);

		bullets.clear();
		enemies.clear();
		healBonuses.clear();

		gamePaused = fromPause;

		scores = 0;
		bulletDirection = Vector2f(0, 0);

		uiManager->setRecordText(recordManager->getRecord());
		uiManager->setHealthText(Hero->getCurrentHealth());
		uiManager->setScoreText(scores);

		if (fromPause)
		{
			RectangleShape PauseRect;
			PauseRect.setSize(Vector2f(MainWindowWidth, MainWindowHeight));
			PauseRect.setFillColor(Color(0, 0, 0, 50));
			PauseRect.setPosition(Vector2f(0, 0));

			MainWindow.draw(PauseRect);
			MainWindow.draw(uiManager->getRecordText());
			MainWindow.display();
		}
	}

	void pauseGame()
	{
		gamePaused = isGamePaused();
		RectangleShape PauseRect;
		PauseRect.setSize(Vector2f(MainWindowWidth, MainWindowHeight));
		PauseRect.setFillColor(Color(0, 0, 0, 50));
		PauseRect.setPosition(Vector2f(0, 0));
		MainWindow.draw(PauseRect);
		MainWindow.display();
	}

	void setBulletDirection()
	{
		if (bulletDirection != Vector2f(0, 0) && (bulletTimePassed >= keyDelay)) 
		{
			if (bulletDirection.y != 0 && bulletDirection.x != 0) 
			{
				bulletDirection.x *= sqrt(2) / 2;
				bulletDirection.y *= sqrt(2) / 2;
			}
			bullets.push_back(Bullet(Hero->x, Hero->y, bulletDirection)); 
			bulletTimePassed = 0.0f;
			audioManager->PlayShootSound();
		}
		bulletDirection = Vector2f(0, 0);
	}

	void input()
	{
		if (Keyboard::isKeyPressed(Keyboard::Key::N) && (timePassed >= keyDelay + 0.3f)) 
		{
			audioManager->SetNextSoundTrack();
			timePassed = 0.0f;
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::V) && (timePassed >= keyDelay + 0.3f))
		{
			audioManager->SetSoundTrackVolume();
			timePassed = 0.0f;
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::B) && (timePassed >= keyDelay + 0.3f)) 
		{
			audioManager->SetSoundVolume();
			timePassed = 0.0f;
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::P) && (timePassed >= keyDelay + 0.3f)) 
		{
			restartGame(true);
			timePassed = 0.0f;
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::Space) && (timePassed >= keyDelay + 0.3f))
		{
			pauseGame();
			timePassed = 0.0f;
		}

		if (Keyboard::isKeyPressed(Keyboard::Key::Escape) && (timePassed >= keyDelay + 0.3f))
		{
			restartGame(false);
			menu();
		}

		if (Keyboard::isKeyPressed(Keyboard::Key::Left)) { bulletDirection.x = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) { bulletDirection.x = 1; }
		if (Keyboard::isKeyPressed(Keyboard::Key::Up)) { bulletDirection.y = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) { bulletDirection.y = 1; }
	}

	void update(float dtAsSeconds)
	{
		Hero->update(dtAsSeconds);
		UpdateBullets(dtAsSeconds);
		UpdateEnemies(dtAsSeconds);
		UpdateHealBonuses();
	}

	void draw()
	{
		MainWindow.clear();
		MainWindow.draw(*MainBackgroundSprite);
		for (int i = healBonuses.size() - 1; i >= 0; i--)
		{
			MainWindow.draw(healBonuses[i].sprite);
		}
		MainWindow.draw(Hero->sprite);
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			MainWindow.draw(bullets[i].sprite);
		}
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			MainWindow.draw(enemies[i].sprite);
		}
		MainWindow.draw(uiManager->getHealthText());
		MainWindow.draw(uiManager->getScoreText());
		MainWindow.display(); 
	}

public:
	Engine()
	{
		MainWindow.create(VideoMode(Vector2u(MainWindowWidth, MainWindowHeight)), "2D-Shooter");
		MainWindow.setPosition(Vector2i(20, 15));
		MainWindow.setFramerateLimit(fps);
		MainBackgroundTexture = new Texture("images/background.png");
		MainBackgroundSprite = new Sprite(*MainBackgroundTexture);
		audioManager = new AudioManager("kick3.ogg");
		recordManager = new RecordManager(0);
		srand(time(0));
		keyDelay = 0.15f;
		timePassed = 0.0f;
		bulletTimePassed = 0.0f;
		Hero = new Player(400, 200);
		bulletDirection = Vector2f(0, 0);
		gamePaused = false;
		scores = 0;
		uiManager = new UIManager(Hero->getCurrentHealth(), scores);
	}

	void start()
	{
		Clock clock;

		while (MainWindow.isOpen())
		{
			Time dt = clock.restart();
			float dtAsSeconds = dt.asSeconds();
			timePassed += dt.asSeconds();
			bulletTimePassed += dt.asSeconds();

			while (const std::optional event = MainWindow.pollEvent()) {
				if (event->is<Event::Closed>())
				{
					MainWindow.close();
				}
			}

			input();

			if (!gamePaused) 
			{
				update(dtAsSeconds);

				draw();
			}
			if (!Hero->PlayerAlive())
			{
				restartGame(true);
			}
		}
	}

	void menu() {
		Texture menuTexture1("images/play.png"), 
			menuTexture2("images/about.png"), 
			menuTexture3("images/exit.png"), 
			aboutTexture("images/fon.png"), 
			menuBackground("images/background.png");
		Sprite menu1(menuTexture1), 
			menu2(menuTexture2), 
			menu3(menuTexture3), 
			about(aboutTexture), 
			menuBg(menuBackground);
		bool isMenu = true;
		float xPos = 300, yPos = 80;
		menu1.setPosition(Vector2f(xPos, 30));
		menu2.setPosition(Vector2f(xPos, 30 + yPos));
		menu3.setPosition(Vector2f(xPos, 30 + 2 * yPos));
		menuBg.setPosition(Vector2f(0, 0));
		Clock clock;
		int selected = 0;

		while (isMenu && MainWindow.isOpen())
		{
			Time dt = clock.restart();
			timePassed += dt.asSeconds();
			while (const std::optional event = MainWindow.pollEvent()) {
				if (event->is<Event::Closed>())
				{
					MainWindow.close();
				}
			}
			menu1.setColor(Color::White);
			menu2.setColor(Color::White);
			menu3.setColor(Color::White);
			if (selected == 0) { menu1.setColor(Color::Cyan); }
			if (selected == 1) { menu2.setColor(Color::Cyan); }
			if (selected == 2) { menu3.setColor(Color::Cyan); }
			if ((Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)) && (timePassed >= keyDelay)) {
				selected < 2 ? selected++ : selected = 0;
				timePassed = 0.0f;
			}
			if ((Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) && (timePassed >= keyDelay)) {
				selected > 0 ? selected-- : selected = 2;
				timePassed = 0.0f;
			}
			if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
				{
					if (selected == 0) isMenu = false;
					if (selected == 1) { MainWindow.draw(about); MainWindow.display(); while (!Keyboard::isKeyPressed(Keyboard::Key::Escape)); }
					if (selected == 2) { MainWindow.close(); isMenu = false; }
				}
			}
			MainWindow.draw(menuBg);
			MainWindow.draw(menu1);
			MainWindow.draw(menu2);
			MainWindow.draw(menu3);

			MainWindow.display();
		}
	}
};
