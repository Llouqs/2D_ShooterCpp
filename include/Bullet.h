#pragma once
#include "Entity.h"

class Bullet :public Entity { 
private:
	Vector2f direction;
	void control() override
	{
		x += direction.x * Speed;
		y += direction.y * Speed;
	}

public:
	RectangleShape sprite;
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

	void update(float time) override
	{
		control();
		sprite.setPosition(Vector2f(x, y));
	}
};