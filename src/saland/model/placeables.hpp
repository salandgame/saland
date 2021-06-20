/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2017 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://github.com/sago007/saland
===========================================================================
*/

#ifndef PLACEABLES_HPP
#define PLACEABLES_HPP

#include <box2d/box2d.h>
#include <string>
#include <memory>

const float pixel2unit = 32.0f;

class Placeable {
public:
	std::string id = "noid";
	float health = 10.0;
	float X = 20.0;
	float Y = 20.0;
	float Radius = 16.0;
	bool removeMe = false;
	bool destructible = true;
	b2Body* body = nullptr;
	virtual bool isStatic() {
		return true;
	}
};

class MiscItem : public Placeable {
public:
	std::string sprite;
	std::string sprite2;
	std::string id = "unknown";
	std::string name = "Unknamed item";
	double base_value = 1.0;
	bool pickup = false;
};

class Creature : public Placeable {
public:
	float stinema = 10.0;
	float mana = 10.0;
	char direction = 'S';
	bool moving = false;
	float moveX = 0.0;
	float moveY = 0.0;
	virtual bool isStatic() override {
		return false;
	}
};

class Human : public Creature {
public:
	std::string race = "male";
	std::string hair = "";
	std::string pants = "";
	std::string top = "";
	float castTimeRemaining = 0;  //If non-zero then we are casting a spell
	float castTime = 400;    //Number of milliseconds it will take to complete the cast
};

class Monster : public Creature {
public:
	std::string race = "bat";
	float speed = 1.0f;
	// AI logic:
	float aiNextThink = 0.0;
};

class Projectile : public Placeable {
public:
	bool active = true;
	float directionX = 1;
	float directionY = 1;
	float velocity = 1.0f;
	float timeToLive = 2000.0;
	std::shared_ptr<Placeable> fired_by;
};

#endif /* PLACEABLES_HPP */

