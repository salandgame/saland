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

#include <SDL2/SDL_timer.h>

#include "Game.hpp"
#include "model/World.hpp"
#include "../sagotmx/tmx_struct.h"
#include "../sago/SagoMisc.hpp"
#include "globals.hpp"
#include "model/placeables.hpp"
#include "SDL.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <condition_variable>

int32 velocityIterations = 6;
int32 positionIterations = 2;

class Projectile : public Placeable {
public:
	bool active = true;
	float directionX = 1;
	float directionY = 1;
	float velocity = 1.0f;
	float timeToLive = 2000.0;
	std::shared_ptr<Placeable> fired_by;
};

/**
 * This sort method sorts the elements from furthest to screen to closest to screen, so that elemnets closer to the screen will be drawn last
 * @param lhs Left hand side
 * @param rhs Right hand side
 * @return true if lhs < rhs
 */
static bool sort_placeable(const std::shared_ptr<Placeable> &lhs, const std::shared_ptr<Placeable> &rhs) {
	return lhs->Y < rhs->Y;
}

static void Draw(SDL_Renderer* target, SDL_Texture* t, int x, int y, const SDL_Rect& part) {
	SDL_Rect pos = {};
	pos.x = x;
	pos.y = y;
	pos.w = 32;
	pos.h = 32;
	SDL_RenderCopy(target, t, &part, &pos);
}

static void DrawLayer(SDL_Renderer* renderer, SDL_Texture* texture, const sago::tiled::TileMap& tm, size_t layer, int topx, int topy) {
	for (int i = 0; i < tm.height; ++i) {
		for (int j = 0; j < tm.width; ++j) {
			uint32_t gid = sago::tiled::getTileFromLayer(tm, tm.layers.at(layer), i, j);
			if (gid == 0) {
				continue;
			}
			SDL_Rect part{};
			getTextureLocationFromGid(tm, gid, nullptr, &part.x, &part.y, &part.w, &part.h);
			Draw(renderer, texture, 32 * i - topx, 32 * j - topy, part);
		}
	}
}

static void DrawOjbectGroup(SDL_Renderer* renderer, const sago::tiled::TileMap& tm, size_t object_group, int topx, int topy) {
	const sago::tiled::TileObjectGroup& group = tm.object_groups.at(object_group);
	for (const sago::tiled::TileObject& o : group.objects) {
		if (o.isEllipse) {
			ellipseRGBA(renderer, (o.x + o.width / 2) - topx, (o.y + o.height / 2) - topy, o.width / 2, o.height / 2, 255, 255, 0, 255);
		} else if (o.polygon_points.size() > 0) {
			for (size_t i = 0; i < o.polygon_points.size(); ++i) {
				std::pair<int, int> first = o.polygon_points.at(i);
				std::pair<int, int> second = (i + 1 < o.polygon_points.size()) ? o.polygon_points.at(i + 1) : o.polygon_points.at(0);
				lineRGBA(renderer, first.first + o.x - topx, first.second + o.y - topy, second.first + o.x - topx, second.second + o.y - topy, 255, 255, 0, 255);
			}
		} else {
			rectangleRGBA(renderer, o.x - topx, o.y - topy,
				o.x + o.width - topx, o.y + o.height - topy, 255, 255, 0, 255);
		}
	}
}

struct Game::GameImpl {
	std::vector<std::shared_ptr<Placeable> > placeables;
	std::shared_ptr<Human> human;
	std::shared_ptr<b2World> physicsBox;
	float center_x = 0;
	float center_y = 0;
	bool drawCollision = true;
	World world;
	int topx = 0.0;
	int topy = 0.0;
	int world_mouse_x = 0; //Mouse cooridinates relative to the world
	int world_mouse_y = 0;
	char direction = 0;
	Uint32 lastUpdate = 0;
};

Game::Game() {
	data.reset(new Game::GameImpl());
	b2Vec2 gravity(0.0f, 0.0f);
	data->physicsBox.reset(new b2World(gravity));
	data->world.init(data->physicsBox);
	data->lastUpdate = SDL_GetTicks();
	data->human.reset(new Human());
	std::shared_ptr<MiscItem> barrel = std::make_shared<MiscItem>();
	barrel.get()->Radius = 16.0f;
	barrel.get()->sprite = "item_barrel";
	barrel.get()->X = 100.0f;
	barrel.get()->Y = 100.0f;
	data->placeables.push_back(barrel);


	std::shared_ptr<Monster> bat = std::make_shared<Monster>();
	bat.get()->Radius = 16.0f;
	bat.get()->race = "bat";
	bat.get()->X = 200.0f;
	bat.get()->Y = 200.0f;
	data->placeables.push_back(bat);
	data->placeables.push_back(data->human);


	b2BodyDef myBodyDef;
	myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
	myBodyDef.position.Set(0, 0);
	myBodyDef.linearDamping = 1.0f;
	data->human->body = data->physicsBox->CreateBody(&myBodyDef);

	b2CircleShape circleShape;
	circleShape.m_p.Set(0, 0); //position, relative to body position
	circleShape.m_radius = 0.5f; //radius 16 pixel (32 pixel = 1)
	b2FixtureDef myFixtureDef;
	myFixtureDef.shape = &circleShape; //this is a pointer to the shape above
	myFixtureDef.density = 10.0f;
	data->human->body->CreateFixture(&myFixtureDef); //add a fixture to the body
	data->human->body->SetTransform(b2Vec2(data->human->X / 32.0f, data->human->Y / 32.0f),data->human->body->GetAngle());

	b2BodyDef batBodyDef;
	batBodyDef.type = b2_dynamicBody;
	batBodyDef.position.Set(0, 0);
	batBodyDef.linearDamping = 1.0f;
	bat->body = data->physicsBox->CreateBody(&batBodyDef);
	b2FixtureDef batDef;
	batDef.shape = &circleShape;
	batDef.density = 10.0f;
	bat->body->CreateFixture(&batDef);
	bat->body->SetTransform(b2Vec2(bat.get()->X / 32.0f, bat.get()->Y / 32.0f),bat->body->GetAngle());

	b2BodyDef barrelBodyDef;
	barrelBodyDef.type = b2_staticBody;
	barrelBodyDef.position.Set(barrel.get()->X / 32.0f, barrel.get()->Y / 32.0f);
	barrelBodyDef.linearDamping = 1.0f;
	barrel->body = data->physicsBox->CreateBody(&barrelBodyDef);
	barrel->body->CreateFixture(&myFixtureDef);
}

Game::~Game() {
	std::string data2save = sago::tiled::tilemap2string(data->world.tm);
	sago::WriteFileContent("maps/sample1.tmx", data2save);
}

bool Game::IsActive() {
	return true;
}

static void DrawMiscEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const MiscItem *entity, float time,
	int offsetX, int offsetY, bool drawCollision) {
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
	const sago::SagoSprite &mySprite = sHolder->GetSprite(entity->sprite);
	mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
}

static void DrawHumanEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Human *entity, float time, int offsetX, int offsetY, bool drawCollision) {
	std::string animation = "standing";
	bool relativeAnimation = false;
	float relativeAnimationState = 0.0f;
	if (entity->moving) {
		animation = "walkcycle";
	}
	if (entity->castTimeRemaining) {
		animation = "spellcast";
		relativeAnimation = true;
		relativeAnimationState = 1.0f - (entity->castTimeRemaining / entity->castTime);
	}
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
	const sago::SagoSprite &mySprite = sHolder->GetSprite(entity->race + "_" + animation + "_" + std::string(1, entity->direction));
	if (relativeAnimation) {
		mySprite.DrawProgressive(target, relativeAnimationState, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
	} else {
		mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
	}
	/*const sago::SagoSprite &myHair = sHolder->GetSprite(entity->race + "_"+animation+"_hair_1_"+string(1,entity->direction));
	myHair.Draw(target, time, entity->X-offsetX, entity->Y-offsetY);*/
}

static void DrawMonster(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Monster *entity, float time, int offsetX, int offsetY, bool drawCollision) {
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
	const sago::SagoSprite &mySprite = sHolder->GetSprite(entity->race + "_" + std::string(1, entity->direction));
	mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
}


static void DrawProjectile(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Projectile *entity, float time, int offsetX, int offsetY, bool drawCollision) {
	(void)sHolder;
	(void)time;
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
}

static void SetLengthToOne(float& x, float& y) {
	float currentLength = std::sqrt(x*x + y*y);
	x = x/currentLength;
	y = y/currentLength;
}

void Game::Draw(SDL_Renderer* target) {
	data->topx = std::round(data->center_x - 1024.0 / 2.0);
	data->topy = std::round(data->center_y - 768.0 / 2.0);
	if (data->topx < -10) {
		data->topx = -10;
	}
	if (data->topy < -10) {
		data->topy = -10;
	}
	SDL_Texture* texture = globalData.spriteHolder->GetDataHolder().getTexturePtr("terrain");
	for (size_t i = 0; i < data->world.tm.layers.size(); ++i) {
		DrawLayer(target, texture, data->world.tm, i, data->topx, data->topy);
	}
	for (size_t i = 0; i < data->world.tm.object_groups.size(); ++i) {
		DrawOjbectGroup(target, data->world.tm, i, data->topx, data->topy);
	}
	int mousebox_x = data->world_mouse_x - data->world_mouse_x % 32 - data->topx;
	int mousebox_y = data->world_mouse_y - data->world_mouse_y % 32 - data->topy;
	rectangleRGBA(globalData.screen, mousebox_x, mousebox_y,
		mousebox_x + 32, mousebox_y + 32, 255, 255, 0, 255);

	//Draw
	for (const auto& p : data->placeables) {
		MiscItem* m = dynamic_cast<MiscItem*> (p.get());
		if (m) {
			DrawMiscEntity(target, globalData.spriteHolder.get(), m, SDL_GetTicks(), data->topx, data->topy, data->drawCollision);
		}
		Human* h = dynamic_cast<Human*> (p.get());
		if (h) {
			DrawHumanEntity(target, globalData.spriteHolder.get(), h, SDL_GetTicks(), data->topx, data->topy, data->drawCollision);
		}
		Monster* monster = dynamic_cast<Monster*> (p.get());
		if (monster) {
			DrawMonster(target, globalData.spriteHolder.get(), monster, SDL_GetTicks(), data->topx, data->topy, data->drawCollision);
		}
		Projectile* projectile = dynamic_cast<Projectile*> (p.get());
		if (projectile) {
			DrawProjectile(target, globalData.spriteHolder.get(), projectile, SDL_GetTicks(), data->topx, data->topy, true);
		}
	}
}

void Game::ProcessInput(const SDL_Event& event, bool& processed) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_SPACE) {
			if (data->human->castTimeRemaining == 0) {
				data->human->castTimeRemaining = data->human->castTime;
				std::shared_ptr<Projectile> projectile = std::make_shared<Projectile>();
				projectile->X = data->human->X;
				projectile->Y = data->human->Y;
				projectile->Radius = 8.0f;
				projectile->directionX = projectile->X - data->world_mouse_x;
				projectile->directionY = projectile->Y - data->world_mouse_y;
				SetLengthToOne(projectile->directionX, projectile->directionY);
				projectile->fired_by = data->human;
				data->placeables.push_back(projectile);
			}
			processed = true;
		}
	}
}

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

static void UpdateHuman(Human *entity, float fDeltaTime) {
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

static void UpdateMonster(Monster *entity) {
	SetCreatureMovementEntity(entity, entity->moveX, entity->moveY);
	b2Vec2 place = entity->body->GetPosition();
	entity->X = place.x*pixel2unit;
	entity->Y = place.y*pixel2unit;
	if (entity->health <= 0.0) {
		entity->removeMe = true;
	}
}

static void UpdateProjectile(Projectile *entity, float fDeltaTime) {
	entity->timeToLive -= fDeltaTime;
	if (entity->timeToLive < 0.0f) {
		entity->removeMe = true;
		return;
	}
	entity->X -= entity->directionX*fDeltaTime*entity->velocity;
	entity->Y -= entity->directionY*fDeltaTime*entity->velocity;
}

static bool Intersect(const Placeable& p1, const Placeable& p2) {
	double distance = std::sqrt( std::pow(p1.X-p2.X, 2) + std::pow(p1.Y-p2.Y,2));
	if (distance < p1.Radius + p2.Radius) {
		return true;
	}
	return false;
}

static void ProjectileHit(Projectile* p, Placeable* target) {
	Monster* monster = dynamic_cast<Monster*> (target);
	if (monster) {
		monster->health -= 10.0f;
		p->removeMe = true;
	}
}

void Game::Update() {
	Uint32 nowTime = SDL_GetTicks();
	Uint32 deltaTime = nowTime - data->lastUpdate;
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	float deltaX = 0.0f;
	float deltaY = 0.0f;
	if (state[SDL_SCANCODE_DOWN]) {
		deltaY += 1.0f;
	}
	if (state[SDL_SCANCODE_UP]) {
		deltaY -= 1.0f;
	}
	if (state[SDL_SCANCODE_RIGHT]) {
		deltaX += 1.0f;
	}
	if (state[SDL_SCANCODE_LEFT]) {
		deltaX -= 1.0f;
	}
	data->human->moveX = deltaX;
	data->human->moveY = deltaY;
	UpdateHuman(data->human.get(), deltaTime);
	for (std::shared_ptr<Placeable>& entity : data->placeables) {
		if (entity->removeMe) {
			continue;
		}
		Projectile* projectile = dynamic_cast<Projectile*> (entity.get());
		if (projectile) {
			UpdateProjectile(projectile, deltaTime);
			for (std::shared_ptr<Placeable>& target : data->placeables) {
				if (target->removeMe) {
					continue;
				}
				if (entity != target && Intersect(*entity.get(), *target.get())) {
					if (projectile->fired_by.get() == target.get()) {
						continue;
					}
					std::cout << "Hit\n" << entity->X << "," << entity->Y << " " << target->X << "," << target->Y << "\n";
					ProjectileHit(projectile, target.get());
				}
			}
		}
		Monster* monster = dynamic_cast<Monster*> (entity.get());
		if (monster) {
			UpdateMonster(monster);
		}
	}
	auto& vp = data->placeables;
	vp.erase(std::remove_if(std::begin(vp), std::end(vp), [](std::shared_ptr<Placeable> p) { return p->removeMe; }), std::end(vp));
	data->center_x = std::round(data->human->X);
	data->center_y = std::round(data->human->Y);
	int mousex;
	int mousey;
	SDL_GetMouseState(&mousex, &mousey);
	data->world_mouse_x = data->topx + mousex;
	data->world_mouse_y = data->topy + mousey;
	//std::cout << "world x: " << data->world_mouse_x << ", y: " << data->world_mouse_y << "             \r";
	data->lastUpdate = nowTime;
	data->physicsBox->Step(deltaTime / 1000.0f / 60.0f, velocityIterations, positionIterations);
	std::sort(data->placeables.begin(), data->placeables.end(), sort_placeable);
}
