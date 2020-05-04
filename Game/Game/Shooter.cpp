#include <SFML/Graphics.hpp>
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
const int WindowHeight = 650, WindowWidth = 800; //Размер окна не изменяется и используется везде


class Entity { //абстрактный класс Сущность для всех движущихся объектов
public:
	float x, y; //у любого объекта есть текущая координата в 2D-пространстве
	float Speed; //т.к. класс описывает движущиеся объекты
	String name; //любой Объект имеет имя

protected:
	Entity(float X, float Y, String Name) { //конструктор защищен от вызова извне т.к. описывает абстрактный класс
		x = X; y = Y; name = Name;
	}

	virtual void update(float time) = 0; //виртуальная функция обновления состояния объекта (каждый объект имеет собственное поведение)
	virtual void control() = 0; //виртуальная функция управления объектом (каждый объект имеет собственное управление)
};

class Bullet :public Entity {
public:
	RectangleShape sprite;
	Bullet(float X, float Y, String Name) : Entity(X, Y, Name)
	{
		Speed = 15.0;
		sprite.setSize(Vector2f(5.f, 5.f));
		sprite.setFillColor(Color(0, 0, 0, 255));
	}
};

class Player :public Entity { //класс игрока
public:
	int health;
	bool isAlive;
	RectangleShape sprite;

	Player(float X, float Y, String Name) : Entity(X, Y, Name) {
		isAlive = true;
		Speed = 7.0;
		sprite.setSize(Vector2f(25.f, 25.f));
		sprite.setFillColor(Color(100, 0, 210, 255));
		sprite.setOrigin(-12.5f, -12.5f);
	}

	void control() override
	{
		if (Keyboard::isKeyPressed) {//если нажата клавиша
			if (Keyboard::isKeyPressed(Keyboard::W)) { if (y > -5) y -= Speed; }
			if (Keyboard::isKeyPressed(Keyboard::A)) { if (x > 0) x -= Speed; }
			if (Keyboard::isKeyPressed(Keyboard::S)) { if (y < WindowHeight - 25) y += Speed; }
			if (Keyboard::isKeyPressed(Keyboard::D)) { if (x < WindowWidth - 35) x += Speed; }
		}
	}

	void update(float time) override
	{
		control();//функция управления персонажем
		sprite.setPosition(x, y);
		if (health <= 0) { isAlive = false; }
	}
};

class Enemy :public Entity {
public:
	float PlayerX, PlayerY;
	int health;
	bool isAlive;
	CircleShape sprite;

	Enemy(float X, float Y, String Name) : Entity(X, Y, Name) {
		isAlive = true;
		Speed = 2.0;
		sprite.setRadius(25.f);
		sprite.setFillColor(Color(180, 25, 195, 255));
	}

	float hypotenuse() {
		return sqrt((PlayerX - x)*(PlayerX - x) + (PlayerY - y)*(PlayerY - y)); //квадратный корень суммы квадратов катетов
	}

	void control() override {
		x += Speed * ((PlayerX - x) / hypotenuse()); //все что после знака * это cos(растояние по x / растояние в общем)
		y += Speed * ((PlayerY - y) / hypotenuse()); //все что после знака * это sin(растояние по y / растояние в общем)
	}

	void GetPlayerCoord(float px, float py)
	{
		PlayerX = px;
		PlayerY = py;
	}

	bool CheckCollision(float px, float py) //проверка на столкновения
	{
		return sprite.getGlobalBounds().intersects(FloatRect(Vector2f(px + 17.5f, py + 17.5f), Vector2f(10.f, 10.f)));
	}

	void update(float time) override
	{
		if (!CheckCollision(PlayerX, PlayerY))
			control();//функция управления персонажем
		sprite.setPosition(x, y);
		if (health <= 0) { isAlive = false; }
	}
};

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

void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("images/play.png");
	menuTexture2.loadFromFile("images/about.png");
	menuTexture3.loadFromFile("images/exit.png");
	aboutTexture.loadFromFile("images/fon.png"); //сюда чтото
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

bool startGame()
{
	srand(time(0));
	Clock clock;
	int x = 400, y = 300;
	int fps;
	long PlayerScores = 0;
	RenderWindow window(VideoMode(WindowWidth, WindowHeight), "2D-Shooter");
	window.setPosition(sf::Vector2i(20, 15));
	window.setFramerateLimit(60);

	Player Hero(400, 200, "Hero");
	Enemy TestEnemy(100, 100, "Small");
	Enemy TestEnemy2(400, 400, "Small2");

	Font TextFont;
	TextFont.loadFromFile("images/arial.ttf");
	Text RecordText("", TextFont, 20);
	RecordText.setColor(Color::White);
	RecordText.setStyle(Text::Bold);

	Texture BackgroundTexture;
	BackgroundTexture.loadFromFile("images/background.png");
	Sprite BackgroundSprite(BackgroundTexture);

	menu(window);//вызов меню
	Event event;

	while (window.isOpen())
	{
		if (Keyboard::isKeyPressed(Keyboard::Space)) { return true; }//если пробел, то перезагружаем игру
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; }//если эскейп, то выходим из игры
		fps = 60;
		float time = clock.getElapsedTime().asMicroseconds(); //дать прошедшое время в микросекундах
		clock.restart(); //перезагружает время
		time /= 600; //скорость игры
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		//Update()
		Hero.update(time);
		TestEnemy.GetPlayerCoord(Hero.x, Hero.y);
		TestEnemy.update(time);
		TestEnemy2.GetPlayerCoord(Hero.x, Hero.y);
		TestEnemy2.update(time);
		window.clear(); //очищает экран
		window.draw(BackgroundSprite);

		if (y > (WindowHeight - 100))
		{
			std::ostringstream BestRecord;
			record(PlayerScores);
			long rec = FileElem();
			BestRecord << rec;
			RecordText.setString("Best record: " + BestRecord.str() + "    Press Space to restart");//задаем строку тексту и вызываем сформированную выше строку методом .str() 
			RecordText.setPosition(200, WindowHeight - 60);
			window.draw(RecordText);   //рисует этот текст
		}

		std::ostringstream CurrentRecord;
		CurrentRecord << PlayerScores;		//занесли в переменную число очков, то есть формируем строку
		RecordText.setString("Scores: " + CurrentRecord.str());//задаем строку тексту и вызываем сформированную выше строку методом .str() 
		RecordText.setPosition(10, 0);

		//Отрисовка
		window.draw(RecordText);   //рисует этот текст

		window.draw(TestEnemy.sprite); //рисует объект
		window.draw(TestEnemy2.sprite); //рисует объект
		window.draw(Hero.sprite); //рисует объект
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
