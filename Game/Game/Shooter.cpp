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

using namespace sf; //������������ ���� SFML
const int WindowHeight = 650, WindowWidth = 800; //������ ���� �� ���������� � ������������ �����, �������� ��� � ����� engine
float PlayerX, PlayerY;

class Entity { //����������� ����� �������� ��� ���� ���������� ��������
public:
	float x, y; //� ������ ������� ���� ������� ���������� � 2D-������������
	float Speed; //�.�. ����� ��������� ���������� �������

protected:
	Entity(float X, float Y) { //����������� ������� �� ������ ����� �.�. ��������� ����������� �����
		x = X; y = Y;
	}

	virtual void update(float time) = 0; //����������� ������� ���������� ��������� ������� (������ ������ ����� ����������� ���������)
	virtual void control() = 0; //����������� ������� ���������� �������� (������ ������ ����� ����������� ����������)
};

class Bullet :public Entity { //����� � ����������
public:
	RectangleShape sprite;
	Vector2f direction;
	Bullet(float X, float Y, Vector2f dir) : Entity(X, Y)
	{
		Speed = 15.0;
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

class Player :public Entity { //����� ������
public:
	int health; //���� �� �������� ����
	bool isAlive; //���� �� �������� ����
	RectangleShape sprite;

	Player(float X, float Y) : Entity(X, Y) {
		isAlive = true;
		Speed = 7.0;
		sprite.setSize(Vector2f(25.f, 25.f)); //25�25 �������
		sprite.setFillColor(Color(100, 0, 210, 255)); //���������� ���� (�� ����������)
		sprite.setOrigin(10.5f, 10.5f); //����� � �������� �������
	}

	void control() override
	{
		if (Keyboard::isKeyPressed(Keyboard::W)) { if (y > 0) y -= Speed; } //������� ������ �������� �� �� ����� ����� �� ����� �� ����
		if (Keyboard::isKeyPressed(Keyboard::A)) { if (x > 0) x -= Speed; }
		if (Keyboard::isKeyPressed(Keyboard::S)) { if (y < WindowHeight) y += Speed; }
		if (Keyboard::isKeyPressed(Keyboard::D)) { if (x < WindowWidth) x += Speed; }
	}

	void update(float time) override
	{
		control();//������� ���������� ����������
		sprite.setPosition(x, y); //� ������ ����������� � ������ ����� ���� ����� �������
		if (health <= 0) { isAlive = false; } //���� �� �����������!
	}
};

class Enemy :public Entity { //����� ����� ����� �� ����� ������
public:
	int health;
	bool isAlive;
	CircleShape sprite;

	Enemy(float X, float Y) : Entity(X, Y) {
		isAlive = true;
		Speed = 2.0;
		health = 50;
		sprite.setRadius(25.f); //����� ���������� ������ ��� �������� �.�. � �������� 50
		sprite.setFillColor(Color(180, 25, 195, 255)); //���� �������
		sprite.setOrigin(12.5f, 12.5f);
	}

	float hypotenuse() {
		return sqrt((PlayerX - x)*(PlayerX - x) + (PlayerY - y)*(PlayerY - y)); //���������� ������ ����� ��������� ������� 
	}

	void control() override {
		//����������� ��������������� ������, ������� � ������ ����������� ��������, ������������ ���������� ������
		x += Speed * ((PlayerX - x) / hypotenuse()); //��� ��� ����� ����� * ��� cos(��������� �� x / ��������� � �����)
		y += Speed * ((PlayerY - y) / hypotenuse()); //��� ��� ����� ����� * ��� sin(��������� �� y / ��������� � �����)
	}

	bool CheckCollision(float px, float py) //�������� �� ������������
	{
		//������� ���������� -1 ���� ������ ����� �� ���������� �������� �������
		return sprite.getGlobalBounds().intersects(FloatRect(Vector2f(px, py), Vector2f(10.f, 10.f)));
	}

	bool CheckHit(float px, float py)
	{
		return sprite.getGlobalBounds().intersects(FloatRect(Vector2f(px, py), Vector2f(10.f, 10.f)));
	}

	void update(float time) override
	{
		if (CheckHit(PlayerX, PlayerY)) 
		{
		health -= 1;
		sprite.setFillColor(Color(180, 25, 195, health * 5));
		}
		if (!CheckCollision(PlayerX, PlayerY)&&isAlive) //����� �� ����� ��������� ����� �� �������� �� ������
			control();//������� ���������� ����������
		sprite.setPosition(x, y);
		if (health <= 0) 
		{ 
			isAlive = false; 
			sprite.setFillColor(Color(180, 225, 195, 255));
		} //�� ������������!
	}
};


class Engine { //����� ������ ��� �� ����������!!!
private:
	int fps = 60;
	const int MainWindowHeight = 650, MainWindowWidth = 800;
	RenderWindow MainWindow;
	Sprite MainBackgroundSprite;
	Texture MainBackgroundTexture;
	std::vector<Enemy> enemies; //�������� ����� ��������� ������� ������ (��������: ������� ��������� ����� � ��������� ������� ������������)
	std::vector<Bullet> bullets;
	Player *Hero;

	void input()
	{
		if (Keyboard::isKeyPressed(Keyboard::Space)) {}//���� ������, �� ������������� ����
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { MainWindow.close(); }//���� ������, �� ������� �� ����
		if (Keyboard::isKeyPressed(Keyboard::Left)) {}
		if (Keyboard::isKeyPressed(Keyboard::Right)) {}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {}
		if (Keyboard::isKeyPressed(Keyboard::Down)) {}
		//if (Keyboard::isKeyPressed(Keyboard::W)) { if (y > -5) y -= Speed; } //������� ������ �������� �� �� ����� ����� �� ����� �� ����
		//if (Keyboard::isKeyPressed(Keyboard::A)) { if (x > 0) x -= Speed; }
		//if (Keyboard::isKeyPressed(Keyboard::S)) { if (y < WindowHeight - 25) y += Speed; }
		//if (Keyboard::isKeyPressed(Keyboard::D)) { if (x < WindowWidth - 35) x += Speed; }
	}

	void update(float dtAsSeconds)
	{
		//�����
		Hero->update(dtAsSeconds);

		//����
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			bullets[i].update(dtAsSeconds);
		}

		//�����
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
		MainWindow.display(); // �� ��� ����������
	}

public:
	// ����������� ������
	Engine()
	{
		Hero = new Player(400, 200);
		srand(time(0));
		//�������� � ��������� ����
		MainWindow.create(VideoMode(MainWindowWidth, MainWindowHeight), "2D-Shooter");
		MainWindow.setPosition(sf::Vector2i(20, 15));
		MainWindow.setFramerateLimit(fps);
		//�������� ����
		MainBackgroundTexture.loadFromFile("images/background.png");
		MainBackgroundSprite.setTexture(MainBackgroundTexture);
	}

	// ������� ����� ������� ��� ��������� �������
	void start()
	{
		// ������ �������
		Clock clock;

		while (MainWindow.isOpen())
		{
			// ������������� ������ � ���������� ���������� ����� � dt
			Time dt = clock.restart();
			float dtAsSeconds = dt.asSeconds();

			input();
			update(dtAsSeconds);
			draw();
		}
	}

};


//������� �������������� � ��������
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

//���� (����������� ������ ������ Engine)

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

	///����///
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

//�������� ���� ���� (����������� ������ ������ Engine)

bool startGame()
{
	//�������������
	srand(time(0));
	Clock clock;
	float keyDelay = 0.15f;
	float timePassed = 0.0f;
	int fps = 60;
	//long PlayerScores = 0;
	RenderWindow window(VideoMode(WindowWidth, WindowHeight), "2D-Shooter");
	window.setPosition(sf::Vector2i(20, 15));
	window.setFramerateLimit(fps);

	Player Hero(400, 200);
	Enemy TestEnemy(100, 100);
	Enemy TestEnemy2(400, 400);
	Bullet b1(100, 100, Vector2f(0, 0));
	std::vector<Bullet> bullets;
	Vector2f bulletDirection(0, 0);
	//Font TextFont;
	//TextFont.loadFromFile("images/arial.ttf");
	//Text RecordText("", TextFont, 20);
	//RecordText.setColor(Color::White);
	//RecordText.setStyle(Text::Bold);

	Texture BackgroundTexture;
	BackgroundTexture.loadFromFile("images/background.png");
	Sprite BackgroundSprite(BackgroundTexture);

	menu(window); // ��� ����� ����!

	Event event;

	while (window.isOpen()) //���� ��� ������� ������ �� ���������
	{
		if (Keyboard::isKeyPressed(Keyboard::Space)) { return true; }//���� ������, �� ������������� ����
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; }//���� ������, �� ������� �� ����
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
		PlayerY = Hero.y; //� ���� ��� ��� �����
		TestEnemy.update(time);
		TestEnemy2.update(time);

		if (Keyboard::isKeyPressed(Keyboard::Left)) {bulletDirection.x = -1;}
		else if (Keyboard::isKeyPressed(Keyboard::Right)) {bulletDirection.x = 1;}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {bulletDirection.y = -1;}
		else if (Keyboard::isKeyPressed(Keyboard::Down)) {bulletDirection.y = 1;}

		if (bulletDirection != Vector2f(0, 0) && (timePassed>=keyDelay))
		{
			if (bulletDirection.y != 0 && bulletDirection.x != 0)
			{
				bulletDirection.x *= sqrt(2) / 2;
				bulletDirection.y *= sqrt(2) / 2;
			}
			bullets.push_back(Bullet(PlayerX, PlayerY, bulletDirection));
			bulletDirection = Vector2f(0, 0);
			timePassed = 0;
		}

		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			bullets[i].update(time);
			Vector2f tmp = bullets[i].sprite.getPosition();
			if (tmp.x < 0 || tmp.x > WindowWidth || tmp.y < 0 || tmp.y > WindowHeight)
			{
				bullets.erase(bullets.begin() + i);
			}
		}


		//if (condition())
		//{
		//	std::ostringstream BestRecord;
		//	record(PlayerScores);
		//	long rec = FileElem();
		//	BestRecord << rec;
		//	RecordText.setString("Best record: " + BestRecord.str() + "    Press Space to restart");//������ ������ ������ � �������� �������������� ���� ������ ������� .str() 
		//	RecordText.setPosition(200, WindowHeight - 60);
		//	window.draw(RecordText);   //������ ���� �����
		//}

		//std::ostringstream CurrentRecord;
		//CurrentRecord << PlayerScores;		//������� � ���������� ����� �����, �� ���� ��������� ������
		//RecordText.setString("Scores: " + CurrentRecord.str());//������ ������ ������ � �������� �������������� ���� ������ ������� .str() 
		//RecordText.setPosition(10, 0);


		//���������


		window.clear();
		window.draw(BackgroundSprite);
		//window.draw(RecordText);   //������ ���� �����
		window.draw(TestEnemy.sprite);
		window.draw(TestEnemy2.sprite);
		window.draw(Hero.sprite);
		for (int i = bullets.size() - 1; i >= 0; i--)
		{
			window.draw(bullets[i].sprite);
		}
		window.display(); // �� ��� ����������
	}
	return false;
}

void gameRunning() {//�-��� ������������� ���� , ���� ��� ����������
	if (startGame())
		gameRunning(); //���� startGame() == true, �� �������� ������ �-��� gameRunning(), ������� � ���� ������� ����� �������� startGame() 
}

int main()
{
	gameRunning();//��������� ������� ����
	return 0;
}