#pragma once
#include "Entity.h"

class HealBonus{
private:
	Color spriteColor;

public:
	RectangleShape sprite;
	HealBonus(float x, float y)
	{
		spriteColor = Color(Color::White);
		sprite.setSize(Vector2f(20.f, 20.f));
		sprite.setRotation(degrees(45.f));
		sprite.setFillColor(spriteColor);
		sprite.setOrigin(Vector2f(2.5f, 2.5f));
		sprite.setPosition(Vector2f(x, y));
	}
};