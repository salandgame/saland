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

#include "GameDraw.hpp"
#include "GameUpdates.hpp"
#include "GameRegion.hpp"
#include "Game.hpp"
#include "model/World.hpp"
#include "../sagotmx/tmx_struct.h"
#include "../sago/SagoMisc.hpp"
#include "../sago/SagoTextField.hpp"
#include "globals.hpp"
#include "model/placeables.hpp"
#include "SDL.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <condition_variable>
#include <sstream>
#include "console/Console.hpp"

int32 velocityIterations = 6;
int32 positionIterations = 2;

typedef std::pair<float,float> SpawnPoint;

/**
 * This sort method sorts the elements from furthest to screen to closest to screen, so that elemnets closer to the screen will be drawn last
 * @param lhs Left hand side
 * @param rhs Right hand side
 * @return true if lhs < rhs
 */
static bool sort_placeable(const std::shared_ptr<Placeable> &lhs, const std::shared_ptr<Placeable> &rhs) {
	return lhs->Y < rhs->Y;
}

bool PlaceablesSortLowerY(const std::shared_ptr<Placeable>& i, const std::shared_ptr<Placeable>& j) {
	if (!j) {
		return true;
	}
	if (!i) {
		return false;
	}
	return i->Y+i->Radius > j->Y+j->Radius;
}

struct Game::GameImpl {
	GameRegion gameRegion;
	std::shared_ptr<Human> human;
	float center_x = 0;
	float center_y = 0;
	bool drawCollision = true;
	int topx = 0.0;
	int topy = 0.0;
	int world_mouse_x = 0; //Mouse coordinates relative to the world
	int world_mouse_y = 0;
	char direction = 0;
	Uint32 lastUpdate = 0;
	std::string worldName = "world1";
	sago::SagoTextField bottomField;
	std::shared_ptr<Console> c;
};

static SpawnPoint GetSpawnpoint(const sago::tiled::TileMap& tm) {
	SpawnPoint ret(tm.width/2.0f, tm.height/2.0f);
	std::vector<SpawnPoint> points;
	for (const sago::tiled::TileObjectGroup& og : tm.object_groups) {
		for (const sago::tiled::TileObject& o : og.objects) {
			if (o.type == "playerStart") {
				points.push_back(SpawnPoint(o.x/32.0f, o.y/32.0f));
			}
		}
	}
	if (points.size() > 0) {
		ret = points[rand()%points.size()];
	}
	return ret;
}

void Game::ResetWorld(int x, int y) {
	data->gameRegion.SaveRegion();
	data->gameRegion.Init(x, y, data->worldName);
	data->human.reset(new Human());
	data->gameRegion.placeables.push_back(data->human);
	b2BodyDef myBodyDef;
	myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
	myBodyDef.position.Set(0, 0);
	myBodyDef.linearDamping = 1.0f;
	data->human->body = data->gameRegion.physicsBox->CreateBody(&myBodyDef);
	b2CircleShape circleShape;
	circleShape.m_p.Set(0, 0); //position, relative to body position
	circleShape.m_radius = 0.5f; //radius 16 pixel (32 pixel = 1)
	b2FixtureDef myFixtureDef;
	myFixtureDef.shape = &circleShape; //this is a pointer to the shape above
	myFixtureDef.density = 10.0f;
	data->human->body->CreateFixture(&myFixtureDef); //add a fixture to the body
	std::pair<float,float> spawnpoint = GetSpawnpoint(data->gameRegion.world.tm);
	data->human->body->SetTransform(b2Vec2(spawnpoint.first, spawnpoint.second),data->human->body->GetAngle());
}

Game::Game() {
	data.reset(new Game::GameImpl());
	data->lastUpdate = SDL_GetTicks();
	ResetWorld(0,0);

	data->bottomField.SetHolder(globalData.dataHolder);
	data->bottomField.SetFontSize(20);
}

Game::~Game() {
	data->gameRegion.SaveRegion();
}

bool Game::IsActive() {
	return true;
}

static void SetLengthToOne(float& x, float& y) {
	float currentLength = std::sqrt(x*x + y*y);
	x = x/currentLength;
	y = y/currentLength;
}

static std::string GetLayerInfoForTile(const World& w, int x, int y) {
	std::stringstream ret;
	if (!sago::tiled::tileInBound(w.tm, x, y)) {
		ret << "Out of bound";
		return ret.str();
	}
	for (size_t i = 0; i < w.tm.layers.size(); ++i) {
		const sago::tiled::TileLayer& tl = w.tm.layers[i];
		int tile = sago::tiled::getTileFromLayer(w.tm, tl, x, y);
		if (tile != 0) {
			ret << "(" << tl.name << ":" << tile << ")";
		}
	}
	return ret.str();
}

void Game::Draw(SDL_Renderer* target) {
	double screen_width = globalData.xsize;
	double screen_height = globalData.ysize;
	int screen_boarder = 16;
	data->topx = std::round(data->center_x - screen_width / 2.0);
	data->topy = std::round(data->center_y - screen_height / 2.0);
	if (data->topx < -screen_boarder) {
		data->topx = -screen_boarder;
	}
	if (data->topy < -screen_boarder) {
		data->topy = -screen_boarder;
	}
	if (data->topx+screen_width > data->gameRegion.world.tm.width*32+screen_boarder) {
		data->topx=data->gameRegion.world.tm.width*32+screen_boarder-screen_width;
	}
	if (data->topy+screen_height > data->gameRegion.world.tm.height*32+screen_boarder) {
		data->topy = data->gameRegion.world.tm.height*32+screen_boarder-screen_height;
	}
	std::sort(data->gameRegion.placeables.begin(), data->gameRegion.placeables.end(),sort_placeable);
	SDL_Texture* texture = globalData.spriteHolder->GetDataHolder().getTexturePtr("terrain");
	DrawOuterBorder(target, texture, data->gameRegion.world.tm, data->topx, data->topy, data->gameRegion.outerTile);
	for (size_t i = 0; i < data->gameRegion.world.tm.layers.size(); ++i) {
		DrawLayer(target, globalData.spriteHolder.get(), data->gameRegion.world.tm, i, data->topx, data->topy);
	}
	for (size_t i = 0; i < data->gameRegion.world.tm.object_groups.size(); ++i) {
		DrawOjbectGroup(target, data->gameRegion.world.tm, i, data->topx, data->topy);
	}
	int mousebox_x = data->world_mouse_x - data->world_mouse_x % 32 - data->topx;
	int mousebox_y = data->world_mouse_y - data->world_mouse_y % 32 - data->topy;
	rectangleRGBA(globalData.screen, mousebox_x, mousebox_y,
		mousebox_x + 32, mousebox_y + 32, 255, 255, 0, 255);

	//Draw
	for (const auto& p : data->gameRegion.placeables) {
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
	char buffer[200];
	snprintf(buffer, sizeof(buffer), "world_x = %d, world_y = %d, layer_info:%s",
		data->world_mouse_x/32, data->world_mouse_y/32,
		GetLayerInfoForTile(data->gameRegion.world, data->world_mouse_x/32, data->world_mouse_y/32).c_str()
	);
	data->bottomField.SetText(buffer);
	data->bottomField.Draw(target, 2, screen_height, sago::SagoTextField::Alignment::left, sago::SagoTextField::VerticalAlignment::bottom);
	if (data->c) {
		data->c->Draw(target);
	}
//#if DEBUG
	static unsigned long int Frames;
	static unsigned long int Ticks;
	static char FPS[10];
	static sago::SagoTextField fpsField;
	fpsField.SetHolder(globalData.dataHolder);
	Frames++;
	if (SDL_GetTicks() >= Ticks + 1000) {
		if (Frames > 999) {
			Frames=999;
		}
		snprintf(FPS, sizeof(FPS), "%lu fps", Frames);
		Frames = 0;
		Ticks = SDL_GetTicks();
	}
	fpsField.SetText(FPS);
	fpsField.Draw(globalData.screen, globalData.xsize-4, 4, sago::SagoTextField::Alignment::right);
//#endif
}

void Game::ProcessInput(const SDL_Event& event, bool& processed) {
	if (data->c) {
		data->c->ProcessInput(event, processed);
		if (processed) {
			return;
		}
	}
	if (event.type == SDL_KEYDOWN) {
		int tile_x = data->world_mouse_x/32;
		int tile_y = data->world_mouse_y/32;
		if (event.key.keysym.sym == SDLK_w && sago::tiled::tileInBound(data->gameRegion.world.tm, tile_x, tile_y)) {
			int layer_number = 2; //  Do not hardcode
			uint32_t tile = 485;
			sago::tiled::setTileOnLayerNumber(data->gameRegion.world.tm, layer_number, tile_x, tile_y, tile);
			data->gameRegion.world.init_physics(data->gameRegion.physicsBox);
		}
		if (event.key.keysym.sym == SDLK_e && sago::tiled::tileInBound(data->gameRegion.world.tm, tile_x, tile_y)) {
			int layer_number = 2; //  Do not hardcode
			uint32_t tile = 0;
			sago::tiled::setTileOnLayerNumber(data->gameRegion.world.tm, layer_number, tile_x, tile_y, tile);
			data->gameRegion.world.init_physics(data->gameRegion.physicsBox);
		}
		if (event.key.keysym.sym == SDLK_ESCAPE && event.key.keysym.mod & KMOD_LSHIFT) {
			data->c = std::make_shared<Console>();
		}
	}
}

static bool Intersect(const Placeable& p1, const Placeable& p2) {
	double distance = std::sqrt( std::pow(p1.X-p2.X, 2) + std::pow(p1.Y-p2.Y,2));
	if (distance < p1.Radius + p2.Radius) {
		return true;
	}
	return false;
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
	for (std::shared_ptr<Placeable>& entity : data->gameRegion.placeables) {
		if (entity->removeMe) {
			continue;
		}
		Projectile* projectile = dynamic_cast<Projectile*> (entity.get());
		if (projectile) {
			UpdateProjectile(projectile, deltaTime);
			for (std::shared_ptr<Placeable>& target : data->gameRegion.placeables) {
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
	auto& vp = data->gameRegion.placeables;
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
	data->gameRegion.physicsBox->Step(deltaTime / 1000.0f / 60.0f, velocityIterations, positionIterations);
	std::sort(data->gameRegion.placeables.begin(), data->gameRegion.placeables.end(), sort_placeable);
	if (data->human->X < 0) {
		ResetWorld(data->gameRegion.GetRegionX()-1, data->gameRegion.GetRegionY());
		data->human->body->SetTransform(b2Vec2(data->gameRegion.world.tm.width, data->gameRegion.world.tm.height/2),data->human->body->GetAngle()) ;
	}
	if (data->human->X > data->gameRegion.world.tm.width*32) {
		ResetWorld(data->gameRegion.GetRegionX()+1, data->gameRegion.GetRegionY());
		data->human->body->SetTransform(b2Vec2(1, data->gameRegion.world.tm.height/2),data->human->body->GetAngle()) ;
	}
	if (data->human->Y < 0) {
		ResetWorld(data->gameRegion.GetRegionX(), data->gameRegion.GetRegionY()-1);
		data->human->body->SetTransform(b2Vec2(data->gameRegion.world.tm.width/2, data->gameRegion.world.tm.height),data->human->body->GetAngle()) ;
	}
	if (data->human->Y > data->gameRegion.world.tm.height*32) {
		ResetWorld(data->gameRegion.GetRegionX(), data->gameRegion.GetRegionY()+1);
		data->human->body->SetTransform(b2Vec2(data->gameRegion.world.tm.width/2, 1),data->human->body->GetAngle()) ;
	}
	if (data->c && !data->c->IsActive()) {
		data->c = nullptr;
	}
	if (SDL_GetMouseState(nullptr,nullptr) & 1) {
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
			data->gameRegion.placeables.push_back(projectile);
		}
	}
}
