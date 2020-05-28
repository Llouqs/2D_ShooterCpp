#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <time.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <vector>
#include <list>
#include <math.h>
#include <cmath>

using namespace sf; //пространство имен SFML

class Entity { //абстрактный класс Сущность для всех движущихся объектов
public:
	float x, y; //у любого объекта есть текущая координата в 2D-пространстве
	float Speed; //т.к. класс описывает движущиеся объекты

protected:
	Entity(float X, float Y) { //конструктор защищен от вызова извне т.к. описывает абстрактный класс
		x = X; y = Y;
	}

	virtual void update(float time) = 0; //виртуальная функция обновления состояния объекта (каждый объект имеет собственное поведение)
	virtual void control() = 0; //виртуальная функция управления объектом (каждый объект имеет собственное управление)
};

class Bullet :public Entity { //класс в разработке
private:
	Bullet(const Bullet &bullet, float X, float Y) : Entity(X, Y)
	{}
public:
	RectangleShape sprite;
	Vector2f direction;
	Bullet(float X, float Y, Vector2f dir) : Entity(X, Y)
	{
		Speed = 20.0;
		sprite.setSize(Vector2f(5.f, 5.f)); //5х5 квадрат 
		sprite.setFillColor(Color(255, 255, 255, 255)); //белый непрозрачный
		direction = dir;
	}

	void set_direction(int x, int y)
	{
		direction = Vector2f(x, y);
	}

	void control() override
	{
		x += direction.x * Speed;
		y += direction.y * Speed;
	}

	void update(float time) override
	{
		control();
		sprite.setPosition(x, y);
	}
};

class Player :public Entity { //класс игрока
public:
	int health; //пока не активное поле
	bool isAlive; //пока не активное поле
	RectangleShape sprite;
	Vector2f playerDirection;

	Player(float X, float Y) : Entity(X, Y) {
		isAlive = true;
		Speed = 8.0;
		sprite.setSize(Vector2f(25.f, 25.f)); //25х25 квадрат
		sprite.setFillColor(Color(100, 0, 210, 255)); //фиолетовый цвет (не прозрачный)
		sprite.setOrigin(10.5f, 10.5f); //центр в середине спрайта
	}

	void control() override
	{
		if (Keyboard::isKeyPressed(Keyboard::A)) { if (x > 0) playerDirection.x = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::D)) { if (x < 800) playerDirection.x = 1; }
		if (Keyboard::isKeyPressed(Keyboard::W)) { if (y > 0) playerDirection.y = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::S)) { if (y < 650) playerDirection.y = 1; }
		if (playerDirection != Vector2f(0, 0)) //если направление задано
		{
			if (playerDirection.y != 0 && playerDirection.x != 0) //нормализовать вектор движения героя sin и cos
			{
				playerDirection.x *= sqrt(2) / 2;
				playerDirection.y *= sqrt(2) / 2;
			}
			x += Speed * playerDirection.x;
			y += Speed * playerDirection.y;
		}
		playerDirection = Vector2f(0, 0); //вектор в любом случае обнулить
	}

	void update(float time) override
	{
		control();//функция управления персонажем
		sprite.setPosition(x, y); //с каждым обновлением у игрока может быть новая позиция
		if (health <= 0) { isAlive = false; } //пока не реализовано!
	}
};

class Enemy :public Entity { //класс врага похож на класс игрока
public:
	int currentHealth;
	int maxHealth;
	bool isAlive;
	CircleShape sprite;
	Color spriteColor;
	float radius;
	static Vector2f CurentPlayerPosition;

	Enemy(float X, float Y) : Entity(X, Y) {
		isAlive = true;
		radius = 25.f + rand() % 25 - 12.5f;
		Speed = 80/radius;
		currentHealth = 3 * radius;
		maxHealth = currentHealth;
		spriteColor = Color(rand() % 255, 25, 195, 255); //цвет от синего до розового
		sprite.setRadius(radius); //враги получаются больше чем персонаж т.к. в диаметре 50
		sprite.setFillColor(spriteColor); 
		sprite.setOrigin(12.5f, 12.5f);
	}

	float hypotenuse() {
		return sqrt((CurentPlayerPosition.x - x)*(CurentPlayerPosition.x - x) 
			+ (CurentPlayerPosition.y - y)*(CurentPlayerPosition.y - y)); //квадратный корень суммы квадратов катетов 
	}

	void control() override {
		//вычисляется нормализованный вектор, который и задает направление движения, относительно координате игрока
		x += Speed * ((CurentPlayerPosition.x - x) / hypotenuse()); //все что после знака * это cos(растояние по x / растояние в общем)
		y += Speed * ((CurentPlayerPosition.y - y) / hypotenuse()); //все что после знака * это sin(растояние по y / растояние в общем)
	}

	bool CheckCollision(float px, float py) //проверка на столкновения
	{
		//функция возвразает -1 если спрайт врага не пересекает заданной области
		return sprite.getGlobalBounds().intersects(FloatRect(Vector2f(px, py), Vector2f(10.f, 10.f)));
	}

	void IsHit()
	{
		currentHealth -= 25;
		spriteColor.a = 255 * currentHealth/maxHealth + 20;
		sprite.setFillColor(spriteColor);
	}


	void update(float time) override
	{
		if (!CheckCollision(CurentPlayerPosition.x, CurentPlayerPosition.y)&&isAlive) //врагу не нужно двигаться когда он добрался до игрока
			control();//функция управления персонажем
		sprite.setPosition(x, y);
		if (currentHealth <= 0) 
		{ 
			isAlive = false;
		} //не реализовано!
	}
};

Vector2f Enemy :: CurentPlayerPosition = Vector2f(0, 0);


class AudioManager {
private:
	Music MainSoundTrack;
	SoundBuffer ShootSoundBuffer;
	Sound ShootSound;
	bool SoundOn;

	void SetShootSound(std::string ShootSoundPath)
	{
		if (!ShootSoundBuffer.loadFromFile("music/" + ShootSoundPath))
			std::cout << "Do not found " << ShootSoundPath << " in 'music/'" << std::endl;
		ShootSound = Sound(ShootSoundBuffer);
	}

	void SetSoundTrack(std::string SoundTrackPath)
	{
		if (!MainSoundTrack.openFromFile("music/" + SoundTrackPath))
			std::cout << "Do not found " << SoundTrackPath << " in 'music/'" << std::endl;
	}

public:
	AudioManager(std :: string ShootSoundPath, std :: string SoundTrackPath)
	{
		SetShootSound(ShootSoundPath);
		SetSoundTrack(SoundTrackPath);
		SoundOn = false;
		SetVolume();
		MainSoundTrack.play();
	}

	void SetVolume()
	{
		if (SoundOn) 
		{
			ShootSound.setVolume(10);
			MainSoundTrack.setVolume(15);
			SoundOn = false;
		}
		else
		{
			ShootSound.setVolume(0);
			MainSoundTrack.setVolume(0);
			SoundOn = true;
		}
	}

	void PlayShootSound()
	{
		ShootSound.play();
	}
};

class Engine { //класс движок еще не реализован!!!
private:
	int fps = 70;
	const int MainWindowHeight = 650, MainWindowWidth = 800;
	const int MaxEnemiesCount = 12;
	float keyDelay;
	float timePassed;
	RenderWindow MainWindow;
	Sprite MainBackgroundSprite;
	Texture MainBackgroundTexture;
	AudioManager *audioManager;
	std::vector<Enemy> enemies; //возможно будет несколько классов врагов (например: большие медленные круги и маленькие быстрые треугольники)
	std::vector<Bullet> bullets;
	Player *Hero;
	bool gamePaused;
	Vector2f bulletDirection;

	void CheckCollisionBulletsEnemies(bool enemyIsDead, int i)
	{
		for (int j = bullets.size() - 1; j >= 0; j--)
		{
			if (!enemyIsDead)
			{
				if (enemies[i].sprite.getGlobalBounds().intersects(bullets[j].sprite.getGlobalBounds()))
				{
					enemies[i].IsHit();
					bullets.erase(bullets.begin() + j);
					//std::cout << enemies[i].currentHealth << " :ch " << enemies[i].maxHealth << " :h " << std::endl;
					if (enemies[i].currentHealth <= 0)
					{
						enemyIsDead = true;
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
			if (enemies[i].sprite.getGlobalBounds().intersects(enemies[j].sprite.getGlobalBounds()))
			{
				enemies[j].x += (rand() % 2) - 1;
				enemies[j].y += (rand() % 2) - 1;
			}
		}
	}

	void UpdateEnemies(float dtAsSeconds)
	{
		Enemy::CurentPlayerPosition = Vector2f(Hero->x, Hero->y);
		bool enemyIsDead = false;
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			enemies[i].update(dtAsSeconds);
			CheckCollisionEnemiesEnemies(i);
			CheckCollisionBulletsEnemies(enemyIsDead, i);
			enemyIsDead = false;
		}
		spawnNewEnemies();
	}

	void UpdateBullets(float dtAsSeconds)
	{
		setBulletDirection();
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			bullets[i].update(dtAsSeconds);

			Vector2f tmp = bullets[i].sprite.getPosition(); //чтобы не запрашивать каждый раз для слежующего условия
			if (tmp.x < 0 || tmp.x > MainWindowWidth || tmp.y < 0 || tmp.y > MainWindowHeight) //проверить столкновение со стеной
			{
				bullets.erase(bullets.begin() + i); //уничтожаем объект
			}
		}
	}

	void spawnNewEnemies()
	{
		if (enemies.size() < MaxEnemiesCount) //заспавнивать врага вне экрана если их меньше
		{
			int randX = rand() % MainWindowWidth;
			int randY = rand() % MainWindowHeight;
			randX < MainWindowWidth / 2 ? randX -= MainWindowWidth / 2 + 30 : randX += MainWindowWidth / 2 + 30;
			randY < MainWindowHeight / 2 ? randY -= MainWindowHeight / 2 + 30 : randY += MainWindowHeight / 2 + 30;

			enemies.push_back(Enemy(randX, randY));
		}
	}

	bool isGamePaused()
	{
		return gamePaused == false;
	}

	void setBulletDirection()
	{
		if (bulletDirection != Vector2f(0, 0) && (timePassed >= keyDelay)) //задать направление пуле и выстрелить если прошло 0.2 сек
		{
			if (bulletDirection.y != 0 && bulletDirection.x != 0) //нормализовать вектор движения пули sin и cos
			{
				bulletDirection.x *= sqrt(2) / 2;
				bulletDirection.y *= sqrt(2) / 2;
			}
			bullets.push_back(Bullet(Hero->x, Hero->y, bulletDirection)); //добавить пулю в вектор (положение игрока, направление)
			//std::cout << bullets.size() << std::endl << timePassed << std::endl;
			timePassed = 0.0f; //обнулить таймер задержки
			audioManager->PlayShootSound();
		}
		bulletDirection = Vector2f(0, 0); //вектор в любом случае обнулить.
	}

	void input()
	{
		if (Keyboard::isKeyPressed(Keyboard::V) && (timePassed >= keyDelay)) //volume "on" or "off"
		{
			audioManager->SetVolume();
			timePassed = 0.0f;
		}
		if (Keyboard::isKeyPressed(Keyboard::P)) {}//restart()
		if (Keyboard::isKeyPressed(Keyboard::Space)) //если пробел, то пауза
		{
			gamePaused = isGamePaused();
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) //если эскейп, то выходим из игры
		{ 
			MainWindow.close(); 
		} 

		if (Keyboard::isKeyPressed(Keyboard::Left)) { bulletDirection.x = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::Right)) { bulletDirection.x = 1; }
		if (Keyboard::isKeyPressed(Keyboard::Up)) { bulletDirection.y = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::Down)) { bulletDirection.y = 1; }
	}

	void update(float dtAsSeconds)
	{
		//Игрок
		Hero->update(dtAsSeconds);

		//Пули
		UpdateBullets(dtAsSeconds);

		//Враги
		UpdateEnemies(dtAsSeconds);
	}

	void draw()
	{
		MainWindow.clear();
		MainWindow.draw(MainBackgroundSprite);
		MainWindow.draw(Hero->sprite);
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			MainWindow.draw(bullets[i].sprite);
		}
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			MainWindow.draw(enemies[i].sprite);
		}
		MainWindow.display(); // всё это показывает
	}

public:
	// Конструктор движка
	Engine()
	{
		srand(time(0));
		keyDelay = 0.15f;
		timePassed = 0.0f;
		//создание и настройка окна
		MainWindow.create(VideoMode(MainWindowWidth, MainWindowHeight), "2D-Shooter");
		MainWindow.setPosition(sf::Vector2i(20, 15));
		MainWindow.setFramerateLimit(fps);
		//загрузка фона
		MainBackgroundTexture.loadFromFile("images/background.png");
		MainBackgroundSprite.setTexture(MainBackgroundTexture);
		//загрузка звука
		audioManager = new AudioManager("kick3.ogg", "Moon.ogg");
		Hero = new Player(400, 200);
		bulletDirection = Vector2f(0, 0);
		gamePaused = true;
		
	}

	// Функция старт вызовет все приватные функции
	void start()
	{
		// Расчет времени
		Clock clock;
		Event ClosedEvent;

		while (MainWindow.isOpen())
		{
			// Перезапускаем таймер и записываем отмеренное время в dt
			Time dt = clock.restart();
			float dtAsSeconds = dt.asSeconds();
			timePassed += dt.asSeconds();

			while (MainWindow.pollEvent(ClosedEvent)) {
				if (ClosedEvent.type == Event::Closed) MainWindow.close();
			}

			input();

			//if (!gamePaused)
				update(dtAsSeconds);

			draw();
		}
	}

	///МЕНЮ///

	void menu() {
		Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
		menuTexture1.loadFromFile("images/play.png");
		menuTexture2.loadFromFile("images/about.png");
		menuTexture3.loadFromFile("images/exit.png");
		aboutTexture.loadFromFile("images/fon.png");
		menuBackground.loadFromFile("images/background.png");
		Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
		bool isMenu = true;
		float xPos = 300, yPos = 80;
		menu1.setPosition(xPos, 30);
		menu2.setPosition(xPos, 30 + yPos);
		menu3.setPosition(xPos, 30 + 2 * yPos);
		menuBg.setPosition(0, 0);
		Clock clock;
		int selected = 0;

		while (isMenu && MainWindow.isOpen())
		{
			Time dt = clock.restart();
			timePassed += dt.asSeconds();
			Event event;
			while (MainWindow.pollEvent(event))
			{
				if (event.type == Event::Closed)
					MainWindow.close();
			}
			menu1.setColor(Color::White);
			menu2.setColor(Color::White);
			menu3.setColor(Color::White);
			if (selected == 0) { menu1.setColor(Color::Cyan); }
			if (selected == 1) { menu2.setColor(Color::Cyan); }
			if (selected == 2) { menu3.setColor(Color::Cyan); }
			if ((Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) && (timePassed >= keyDelay)) {
				selected < 2 ? selected++ : selected = 0;
				timePassed = 0.0f;
			}
			if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) && (timePassed >= keyDelay)) {
				selected > 0 ? selected-- : selected = 2;
				timePassed = 0.0f;
			}
			if (Keyboard::isKeyPressed(Keyboard::Enter)) {
				{
					if (selected == 0) isMenu = false;
					if (selected == 1) { MainWindow.draw(about); MainWindow.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
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

//функции взаимодействия с рекордом
long FileElem()
{
	long Element;
	FILE * File;
	fopen_s(&File, "record.txt", "r");
	fscanf(File, "%d", &Element);
	fclose(File);
	return Element;
}

void record(long Scores)
{
	long Element = FileElem();
	FILE * File;

	if (Scores > Element)
	{
		fopen_s(&File, "record.txt", "w");
		fprintf(File, "%d", Scores);
		fclose(File);
	}
}


bool startGame()
{
	Engine GameEngine;
	GameEngine.menu();
	GameEngine.start();
	return false;
}

void gameRunning() {//ф-ция перезагружает игру , если это необходимо
	if (startGame())
		gameRunning(); //если startGame() == true, то вызываем занова ф-цию gameRunning(), которая в свою очередь опять вызывает startGame() 
}

int main()
{
	gameRunning();//запускаем процесс игры
	return 0;
}
