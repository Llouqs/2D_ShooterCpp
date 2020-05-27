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
const int WindowHeight = 650, WindowWidth = 800; //Размер окна не изменяется и используется везде, перемещу его в класс engine
float PlayerX, PlayerY;

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
		sprite.setSize(Vector2f(5.f, 5.f));
		sprite.setFillColor(Color(255, 255, 255, 255));
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
		else if (Keyboard::isKeyPressed(Keyboard::D)) { if (x < WindowWidth) playerDirection.x = 1; }
		if (Keyboard::isKeyPressed(Keyboard::W)) { if (y > 0) playerDirection.y = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::S)) { if (y < WindowHeight) playerDirection.y = 1; }
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
		playerDirection = Vector2f(0, 0); //вектор в любом случае обнулить.

		//if (Keyboard::isKeyPressed(Keyboard::W)) { if (y > 0) y -= Speed; } //условия внутри отвечают за то чтобы игрок не вышел за поле
		//if (Keyboard::isKeyPressed(Keyboard::A)) { if (x > 0) x -= Speed; }
		//if (Keyboard::isKeyPressed(Keyboard::S)) { if (y < WindowHeight) y += Speed; }
		//if (Keyboard::isKeyPressed(Keyboard::D)) { if (x < WindowWidth) x += Speed; }
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
		return sqrt((PlayerX - x)*(PlayerX - x) + (PlayerY - y)*(PlayerY - y)); //квадратный корень суммы квадратов катетов 
	}

	void control() override {
		//вычисляется нормализованный вектор, который и задает направление движения, относительно координате игрока
		x += Speed * ((PlayerX - x) / hypotenuse()); //все что после знака * это cos(растояние по x / растояние в общем)
		y += Speed * ((PlayerY - y) / hypotenuse()); //все что после знака * это sin(растояние по y / растояние в общем)
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
		if (!CheckCollision(PlayerX, PlayerY)&&isAlive) //врагу не нужно двигаться когда он добрался до игрока
			control();//функция управления персонажем
		sprite.setPosition(x, y);
		if (currentHealth <= 0) 
		{ 
			isAlive = false;
		} //не реализовано!
	}
};

class AudioManager {
private:
	Music MainSoundTrack;
	SoundBuffer ShootSoundBuffer;
	Sound ShootSound;

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
	AudioManager(std:: string ShootSoundPath, std:: string SoundTrackPath)
	{
		SetShootSound(ShootSoundPath);
		SetSoundTrack(SoundTrackPath);
		SetSoundVolume(true);
		SetSoundTrackVolume(true);
		MainSoundTrack.play();
	}

	void SetSoundVolume(bool SoundOn)
	{
		SoundOn ? ShootSound.setVolume(10) : ShootSound.setVolume(0);
	}

	void SetSoundTrackVolume(bool SoundOn)
	{
		SoundOn ? MainSoundTrack.setVolume(15) : MainSoundTrack.setVolume(0);
	}

	void PlayShootSound()
	{
		ShootSound.play();
	}
};

class Engine { //класс движок еще не реализован!!!
private:
	int fps = 60;
	const int MainWindowHeight = 650, MainWindowWidth = 800;
	RenderWindow MainWindow;
	Sprite MainBackgroundSprite;
	Texture MainBackgroundTexture;
	AudioManager *audioManager;
	std::vector<Enemy> enemies; //возможно будет несколько классов врагов (например: большие медленные круги и маленькие быстрые треугольники)
	std::vector<Bullet> bullets;
	Player *Hero;
	bool gamePaused;

	void input()
	{
		if (Keyboard::isKeyPressed(Keyboard::P)) {}//restart()
		if (Keyboard::isKeyPressed(Keyboard::Space)) {}//если пробел, то пауза
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { MainWindow.close(); }//если эскейп, то выходим из игры
		if (Keyboard::isKeyPressed(Keyboard::Left)) {}
		if (Keyboard::isKeyPressed(Keyboard::Right)) {}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {}
		if (Keyboard::isKeyPressed(Keyboard::Down)) {}
		if (Keyboard::isKeyPressed(Keyboard::W)) {} //условия внутри отвечают за то чтобы игрок не вышел за поле
		if (Keyboard::isKeyPressed(Keyboard::A)) {}
		if (Keyboard::isKeyPressed(Keyboard::S)) {}
		if (Keyboard::isKeyPressed(Keyboard::D)) {}
	}

	void update(float dtAsSeconds)
	{
		//Игрок
		Hero->update(dtAsSeconds);

		//Пули
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			bullets[i].update(dtAsSeconds);
		}

		//Враги
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			enemies[i].update(dtAsSeconds);
		}
	}

	void draw()
	{
		MainWindow.clear();
		MainWindow.draw(Hero->sprite);
		MainWindow.draw(MainBackgroundSprite);
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
		Hero = new Player(400, 200);
		srand(time(0));
		//создание и настройка окна
		MainWindow.create(VideoMode(MainWindowWidth, MainWindowHeight), "2D-Shooter");
		MainWindow.setPosition(sf::Vector2i(20, 15));
		MainWindow.setFramerateLimit(fps);
		//загрузка фона
		MainBackgroundTexture.loadFromFile("images/background.png");
		MainBackgroundSprite.setTexture(MainBackgroundTexture);
		//загрузка звука
		audioManager = new AudioManager("kick3.wav", "Moon.wav");
	}

	// Функция старт вызовет все приватные функции
	void start()
	{
		// Расчет времени
		Clock clock;

		while (MainWindow.isOpen())
		{
			// Перезапускаем таймер и записываем отмеренное время в dt
			Time dt = clock.restart();
			float dtAsSeconds = dt.asSeconds();

			input();
			if (!gamePaused)
				update(dtAsSeconds);
			draw();
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

//МЕНЮ (планируется внутри класса Engine)

void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("images/play.png");
	menuTexture2.loadFromFile("images/about.png");
	menuTexture3.loadFromFile("images/exit.png");
	aboutTexture.loadFromFile("images/fon.png");
	menuBackground.loadFromFile("images/background.png");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
	bool isMenu = 1;
	float xPos = 300, yPos = 80;
	menu1.setPosition(xPos, 30);
	menu2.setPosition(xPos, 30 + yPos);
	menu3.setPosition(xPos, 30 + 2 * yPos);
	menuBg.setPosition(0, 0);

	///МЕНЮ///
	int selected = 0;
	int key = 0;

	while (isMenu&&window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		if (selected == 0) { menu1.setColor(Color::Cyan); Sleep(110); }
		if (selected == 1) { menu2.setColor(Color::Cyan); Sleep(110); }
		if (selected == 2) { menu3.setColor(Color::Cyan); Sleep(110); }
		if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) {
			if (selected < 2)
				selected++;
			else selected = 0;
		}
		if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) {
			if (selected > 0)
				selected--;
			else selected = 2;
		}
		if (Keyboard::isKeyPressed(Keyboard::Enter)) {
			{
				if (selected == 0) isMenu = false;
				if (selected == 1) { window.draw(about); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
				if (selected == 2) { window.close(); isMenu = false; }
			}
		}
		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);

		window.display();
	}
}

//Основной цикл игры (планируется внутри класса Engine)

bool startGame()
{
	//Инициализация
	srand(time(0));
	Clock clock;
	float keyDelay = 0.15f;
	float timePassed = 0.0f;
	int fps = 70;
	//long PlayerScores = 0;
	RenderWindow window(VideoMode(WindowWidth, WindowHeight), "2D-Shooter");
	window.setPosition(sf::Vector2i(20, 15));
	window.setFramerateLimit(fps);

	Player Hero(400, 200);
	Vector2f bulletDirection(0, 0);
	std::vector<Bullet> bullets;
	std::vector<Enemy> enemies;
	std::vector<FloatRect> colliders;
	bool enemyIsDead = false;
	//Font TextFont;
	//TextFont.loadFromFile("images/arial.ttf");
	//Text RecordText("", TextFont, 20);
	//RecordText.setColor(Color::White);
	//RecordText.setStyle(Text::Bold);

	Texture BackgroundTexture;
	BackgroundTexture.loadFromFile("images/background.png");
	Sprite BackgroundSprite(BackgroundTexture);

	menu(window); // ТУТ вызов меню!

	AudioManager audio("kick3.wav", "Moon.wav");
	bool pause = false;
	Event event;

	while (window.isOpen()) //пока нет условия выхода по проигрышу
	{
		fps = 60;
		Time dt = clock.restart();
		timePassed += dt.asSeconds();
		float time = dt.asSeconds();

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		//Update()

		Hero.update(time);
		PlayerX = Hero.x;
		PlayerY = Hero.y; //я знаю что это плохо

		if (Keyboard::isKeyPressed(Keyboard::Space) && (timePassed >= keyDelay))
		{ 
			if (pause)
			{
				pause = true;
				window.setFramerateLimit(1);
			}
			else
			{
				pause = false;
				window.setFramerateLimit(70);
			}
			timePassed = 0.0f;
		}//если пробел, то перезагружаем игру
		if (Keyboard::isKeyPressed(Keyboard::P)) { return true; }//если пробел, то перезагружаем игру
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; }//если эскейп, то выходим из игры
		if (Keyboard::isKeyPressed(Keyboard::Left)) {bulletDirection.x = -1;}
		else if (Keyboard::isKeyPressed(Keyboard::Right)) {bulletDirection.x = 1;}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {bulletDirection.y = -1;}
		else if (Keyboard::isKeyPressed(Keyboard::Down)) {bulletDirection.y = 1;}

		if (enemies.size() < 12) //заспавнивать врага вне экрана если их меньше 7
		{
			int randX = rand() % WindowWidth;
			int randY = rand() % WindowHeight;
			if (randX < WindowWidth / 2)
				randX -= WindowWidth / 2 + 30;
			else randX += WindowWidth / 2 + 30;
			if (randY < WindowHeight / 2)
				randY -= WindowHeight / 2 + 30;
			else randY += WindowHeight / 2 + 30;
			enemies.push_back(Enemy(randX, randY));
		}

		if (bulletDirection != Vector2f(0, 0) && (timePassed >= keyDelay)) //задать направление пуле и выстрелить если прошло 0.2 сек
		{
			if (bulletDirection.y != 0 && bulletDirection.x != 0) //нормализовать вектор движения пули sin и cos
			{
				bulletDirection.x *= sqrt(2) / 2; 
				bulletDirection.y *= sqrt(2) / 2;
			}
			bullets.push_back(Bullet(PlayerX, PlayerY, bulletDirection)); //добавить пулю в вектор (положение игрока, направление)
			//std::cout << bullets.size() << std::endl << timePassed << std::endl;
			timePassed = 0.0f; //обнулить таймер задержки
			audio.PlayShootSound();
		}
		bulletDirection = Vector2f(0, 0); //вектор в любом случае обнулить.

		for (int i = bullets.size() - 1; i >= 0; i--) 
		{
			bullets[i].update(time); //обновляем пулю на каждом кадре
			Vector2f tmp = bullets[i].sprite.getPosition(); //чтобы не запрашивать каждый раз для слежующего условия
			if (tmp.x < 0 || tmp.x > WindowWidth || tmp.y < 0 || tmp.y > WindowHeight) //проверить столкновение со стеной
			{
				bullets.erase(bullets.begin() + i); //уничтожаем объект
			}
		}

		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			enemies[i].update(time);
			for (int j = bullets.size() - 1; j >= 0; j--)
			{
				if (!enemyIsDead)
					if (enemies[i].sprite.getGlobalBounds().intersects(bullets[j].sprite.getGlobalBounds()))
					{
						enemies[i].IsHit();
						bullets.erase(bullets.begin() + j);
						std::cout << enemies[i].currentHealth << " :ch " << enemies[i].maxHealth << " :h " << std::endl;
						if (enemies[i].currentHealth <= 0)
						{
							enemyIsDead = true;
							enemies.erase(enemies.begin() + i);
						}
					}
			}
			enemyIsDead = false;
		}

		for (int i = enemies.size() - 1; i >= 0; i--) //не очень хорошая проверка на столкновение между врагами
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

		//if (condition())
		//{
		//	std::ostringstream BestRecord;
		//	record(PlayerScores);
		//	long rec = FileElem();
		//	BestRecord << rec;
		//	RecordText.setString("Best record: " + BestRecord.str() + "    Press Space to restart");//задаем строку тексту и вызываем сформированную выше строку методом .str() 
		//	RecordText.setPosition(200, WindowHeight - 60);
		//	window.draw(RecordText);   //рисует этот текст
		//}

		//std::ostringstream CurrentRecord;
		//CurrentRecord << PlayerScores;		//занесли в переменную число очков, то есть формируем строку
		//RecordText.setString("Scores: " + CurrentRecord.str());//задаем строку тексту и вызываем сформированную выше строку методом .str() 
		//RecordText.setPosition(10, 0);


		//Отрисовка


		window.clear();
		window.draw(BackgroundSprite);
		//window.draw(RecordText);   //рисует этот текст
		window.draw(Hero.sprite);
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			window.draw(enemies[i].sprite);
		}
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			window.draw(bullets[i].sprite);
		}
		window.display(); // всё это показывает
	}
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
