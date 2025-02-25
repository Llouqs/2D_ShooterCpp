#pragma once
#include "Entity.h"

class Player :public Entity { 
private:
	Vector2f playerDirection;
	bool isAlive;
	Color spriteColor;
	int currentHealth;
	int maxHealth;
public:
	RectangleShape sprite;

	Player(float X, float Y) : Entity(X, Y) {
		isAlive = true;
		Speed = 8.0;
		maxHealth = 250;
		currentHealth = maxHealth;
		spriteColor = Color(100, 0, 210, 255);
		sprite.setSize(Vector2f(25.f, 25.f)); 
		sprite.setFillColor(spriteColor); 
		sprite.setOrigin(Vector2f(10.5f, 10.5f));
	}

	void setPlayerDirection()
	{
		if (playerDirection != Vector2f(0, 0)) 
		{
			if (playerDirection.y != 0 && playerDirection.x != 0)
			{
				playerDirection.x *= sqrt(2) / 2;
				playerDirection.y *= sqrt(2) / 2;
			}
			x += Speed * playerDirection.x;
			y += Speed * playerDirection.y;
		}
		playerDirection = Vector2f(0, 0);
	}

	void control() override
	{
		if (Keyboard::isKeyPressed(Keyboard::Key::A)) { if (x > 0) playerDirection.x = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::Key::D)) { if (x < 800) playerDirection.x = 1; }
		if (Keyboard::isKeyPressed(Keyboard::Key::W)) { if (y > 0) playerDirection.y = -1; }
		else if (Keyboard::isKeyPressed(Keyboard::Key::S)) { if (y < 650) playerDirection.y = 1; }
	}

	int getCurrentHealth()
	{
		return currentHealth;
	}

	void IsHeal()
	{
		if (currentHealth + 10 > maxHealth)
			currentHealth = maxHealth;
		else
			currentHealth += 10;
		spriteColor.a = 255 - (maxHealth - currentHealth);
		sprite.setFillColor(spriteColor);
	}

	void IsHit()
	{
		if (currentHealth > 0)
			currentHealth -= 1;
		spriteColor.a = 255 - (maxHealth - currentHealth);
		sprite.setFillColor(spriteColor);
	}

	bool PlayerAlive()
	{
		return isAlive;
	}

	void update(float time) override
	{
		control();
		setPlayerDirection();
		sprite.setPosition(Vector2f(x, y));
		if (currentHealth <= 0) { isAlive = false; } 
	}
};