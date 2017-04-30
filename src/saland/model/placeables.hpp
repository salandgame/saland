#ifndef PLACEABLES_HPP
#define PLACEABLES_HPP

#include <string>

class Placeable {
public:
	float X = 20.0;
	float Y = 20.0;
	float Radius = 0.0;
	virtual bool isStatic() {return true; }
};

class MistItem : public Placeable {
public:
	std::string sprite;
};

class Creature : public Placeable {
public:
	float health = 10.0;
	float stinema = 10.0;
	float mana = 10.0;
	char direction = 'S';
	bool moving = false;
	virtual bool isStatic() override { return false; }
};

class Human : public Creature {
public:
	std::string race = "male";
};

#endif /* PLACEABLES_HPP */

