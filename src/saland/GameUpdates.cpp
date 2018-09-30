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

#include "GameUpdates.hpp"

static void SetDesiredVelocity(b2Body* body, float x, float y) {
	b2Vec2 vel = body->GetLinearVelocity();
	float velChangeX = x - vel.x;
	float velChangeY = y - vel.y;
	float impulseX = body->GetMass() * velChangeX; //disregard time factor
	float impulseY = body->GetMass() * velChangeY; //disregard time factor
	//std::cout << body->GetMass() << " " << impulseX  << "\n";
	body->ApplyLinearImpulse(b2Vec2(impulseX, impulseY), body->GetWorldCenter(), true);
}

static void SetCreatureMovementEntity(Creature *entity, float directionX, float directionY) {
	float deltaX = directionX;
	float deltaY = directionY;

	if (deltaX == 0.0f && deltaY == 0.0f) {
		entity->moving = false;
		SetDesiredVelocity(entity->body, 0, 0);
		return;
	}
	entity->moving = true;
	if (deltaX * deltaX + deltaY * deltaY > 1.5f) {
		deltaX *= 0.7071067811865476f; //sqrt(0.5)
		deltaY *= 0.7071067811865476f; //sqrt(0.5)
	}
	if (deltaY > 0.0f) {
		entity->direction = 'S';
	}
	if (deltaY < 0.0f) {
		entity->direction = 'N';
	}
	if (deltaX < 0.0f) {
		entity->direction = 'W';
	}
	if (deltaX > 0.0f) {
		entity->direction = 'E';
	}
	float speed = 500.0f;
	SetDesiredVelocity(entity->body, deltaX*speed, deltaY * speed);
}

void UpdateHuman(Human *entity, float fDeltaTime) {
	if (entity->castTimeRemaining > 0) {
		entity->castTimeRemaining -= fDeltaTime;
	}
	if (entity->castTimeRemaining < 0) {
		entity->castTimeRemaining = 0;
	}
	if (entity->castTimeRemaining != 0) {
		entity->moveX = 0.0f;
		entity->moveY = 0.0f;
	}
	SetCreatureMovementEntity(entity, entity->moveX, entity->moveY);
	b2Vec2 place = entity->body->GetPosition();
	entity->X = place.x*pixel2unit;
	entity->Y = place.y*pixel2unit;
}

void UpdateMonster(Monster *entity) {
	SetCreatureMovementEntity(entity, entity->moveX, entity->moveY);
	b2Vec2 place = entity->body->GetPosition();
	entity->X = place.x*pixel2unit;
	entity->Y = place.y*pixel2unit;
	if (entity->health <= 0.0) {
		entity->removeMe = true;
	}
}

void UpdateProjectile(Projectile *entity, float fDeltaTime) {
	entity->timeToLive -= fDeltaTime;
	if (entity->timeToLive < 0.0f) {
		entity->removeMe = true;
		return;
	}
	entity->X -= entity->directionX*fDeltaTime*entity->velocity;
	entity->Y -= entity->directionY*fDeltaTime*entity->velocity;
}

void ProjectileHit(Projectile* p, Placeable* target) {
	Monster* monster = dynamic_cast<Monster*> (target);
	if (monster) {
		monster->health -= 10.0f;
		p->removeMe = true;
	}
}