#pragma once
#include "Entity.h"

class Enemy :public Entity { 
private:
	int maxHealth;
	bool isAlive;
	Color spriteColor;
	float radius;
	int currentHealth;
public:
	CircleShape sprite;
	static Vector2f CurentPlayerPosition;

	Enemy(float X, float Y) : Entity(X, Y) {
		isAlive = true;
		if (rand() % 100 == 66)
			radius = 200;
		else
			radius = 25.f + rand() % 25 - 12.5f;
		Speed = 80 / radius;
		currentHealth = 3 * radius;
		maxHealth = currentHealth;
		spriteColor = Color(rand() % 255, 25, 195, 255); 
		sprite.setRadius(radius); 
		sprite.setFillColor(spriteColor);
		sprite.setOrigin(Vector2f(12.5f, 12.5f));
	}

	int getCurrentHealth()
	{
		return currentHealth;
	}

	int getMaxHealth()
	{
		return maxHealth;
	}

	float hypotenuse() {
		return sqrt((CurentPlayerPosition.x - x) * (CurentPlayerPosition.x - x)
			+ (CurentPlayerPosition.y - y) * (CurentPlayerPosition.y - y));
	}

	void control() override {
		x += Speed * ((CurentPlayerPosition.x - x) / hypotenuse()); 
		y += Speed * ((CurentPlayerPosition.y - y) / hypotenuse());
	}

	bool CheckCollision(float px, float py)
	{
		return sprite.getGlobalBounds().findIntersection(FloatRect(Vector2f(px, py), Vector2f(10.f, 10.f))).has_value();
	}

	void IsHit()
	{
		currentHealth -= 25;
		if (255 * currentHealth / maxHealth + 20 >= 255)
			spriteColor.a = 255;
		else
			spriteColor.a = 255 * currentHealth / maxHealth + 20;
		sprite.setFillColor(spriteColor);
	}


	void update(float time) override
	{
		if (!CheckCollision(CurentPlayerPosition.x, CurentPlayerPosition.y) && isAlive)
			control();
		sprite.setPosition(Vector2f(x, y));
		if (currentHealth <= 0)
		{
			isAlive = false;
		}
	}
};

Vector2f Enemy::CurentPlayerPosition = Vector2f(0, 0);