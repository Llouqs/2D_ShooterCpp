#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Entity { 
public:
	float x, y; 

protected:
	float Speed; 
	Entity(float x, float y, float Speed=0.f) : x(x), y(y), Speed(Speed) {}
	virtual ~Entity() {}
	virtual void update(float time) = 0; 
	virtual void control() = 0; 
};
