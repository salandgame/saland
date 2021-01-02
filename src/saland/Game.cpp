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
#include "GameShop.hpp"
#include "model/World.hpp"
#include "../sagotmx/tmx_struct.h"
#include "../sago/SagoMisc.hpp"
#include "../sago/SagoTextField.hpp"
#include "globals.hpp"
#include "../common.h"
#include "model/placeables.hpp"
#include "model/spells.hpp"
#include "model/Player.hpp"
#include "../os.hpp"
#include "SDL.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <condition_variable>
#include <sstream>
#include "console/Console.hpp"
#include "GameConsoleCommand.hpp"

int32 velocityIterations = 6;
int32 positionIterations = 2;


typedef std::pair<float,float> SpawnPoint;

/**
 * This sort method sorts the elements from furthest to screen to closest to screen, so that elemnets closer to the screen will be drawn last
 * @param lhs Left hand side
 * @param rhs Right hand side
 * @return true if lhs < rhs
 */
static bool sort_placeable(const std::shared_ptr<Placeable>& lhs, const std::shared_ptr<Placeable>& rhs) {
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

static int string2int_trows(const std::string& s) {
	try {
		return std::stoi(s);
	}
	catch (std::exception&) {
		std::stringstream ss;
		ss << "Failed to convert \"" << s << "\" to an integer";
		throw std::runtime_error(ss.str());
	}
}

static bool reset_region = false;
static bool teleport = false;
static int teleportX = 0;
static int teleportY = 0;
static bool openTiled = false;

struct GotoConsoleCommand : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "goto";
	}
	virtual std::string run(const std::vector<std::string>& args) override {
		if (args.size() != 3) {
			return "Must be ran like \"goto X Y\"";
		}
		teleportX = string2int_trows(args[1]);
		teleportY = string2int_trows(args[2]);
		teleport = true;
		return "Teleport queued!";
	}

	virtual std::string helpMessage() const override {
		return "Call like \"goto <X> <Y>\". Where <X> and <Y> is the regions to go to.";
	}
};

struct ResetRegionConsoleCommand : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "reset_region";
	}
	virtual std::string run(const std::vector<std::string>&) override {
		reset_region = true;
		return "Region reset queued!";
	}

	virtual std::string helpMessage() const override {
		return "Resets the current region back to default";
	}
};

void RunGameState(sago::GameStateInterface& state );

struct ShopCommand : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "shop";
	}
	virtual std::string run(const std::vector<std::string>&) override {
		GameShop gs;
		RunGameState(gs);
		return "Shop!";
	}

	virtual std::string helpMessage() const override {
		return "Opens a shop";
	}
};

struct ConcoleCommandTiled : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "tiled";
	}
	virtual std::string run(const std::vector<std::string>&) override {
		openTiled = true;
		return "Requesting tiled";
	}

	virtual std::string helpMessage() const override {
		return "Opens current region in tiled";
	}
};

static GotoConsoleCommand gcc;
static ResetRegionConsoleCommand rrcc;
static ShopCommand sc;
static ConcoleCommandTiled cct;

struct Game::GameImpl {
	GameRegion gameRegion;
	std::shared_ptr<Human> human;
	float center_x = 0;
	float center_y = 0;
	bool drawCollision = Config::getInstance()->getInt("draw_collision");
	int topx = 0.0;
	int topy = 0.0;
	int world_mouse_x = 0; //Mouse coordinates relative to the world
	int world_mouse_y = 0;
	char direction = 0;
	Uint32 lastUpdate = 0;
	uint32_t drawTile = 607;
	bool isActive = true;
	std::string worldName = "world1";
	sago::SagoTextField bottomField;
	sago::SagoTextField middleField;
	std::array<sago::SagoTextField,10> number_labels;  //Label: 0,1...10
	size_t slot_selected = 0;
	std::array<Spell, 10> slot_spell;
	std::shared_ptr<Console> console;
	bool consoleActive = false;
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

void Game::ResetWorldNoSave(int x, int y, bool forceResetWorld) {
	data->gameRegion.Init(x, y, data->worldName, forceResetWorld);
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

void Game::ResetWorld(int x, int y, bool forceResetWorld) {
	data->gameRegion.SaveRegion();
	this->ResetWorldNoSave(x, y, forceResetWorld);
}

Game::Game() {
	RegisterCommand(&gcc);
	RegisterCommand(&rrcc);
	RegisterCommand(&sc);
	RegisterCommand(&cct);
	GameConsoleCommandRegister();
	data.reset(new Game::GameImpl());
	data->human.reset(new Human());
	data->lastUpdate = SDL_GetTicks();
	ResetWorld(0, 0, false);
	data->human->pants = globalData.player.get_visible_bottom();
	data->human->hair = globalData.player.get_visible_hair();

	data->human->race = globalData.player.get_visible_race();
	data->human->top = globalData.player.get_visible_top();

	data->bottomField.SetHolder(globalData.dataHolder);
	data->bottomField.SetFontSize(20);
	data->middleField.SetHolder(globalData.dataHolder);
	data->middleField.SetFontSize(20);
	for (size_t i = 0; i < data->number_labels.size(); ++i) {
		data->number_labels.at(i).SetHolder(globalData.dataHolder);
		data->number_labels.at(i).SetFontSize(20);
		data->number_labels.at(i).SetText(std::to_string(i));
	}
	Spell& slot0 = data->slot_spell.at(0);
	slot0.icon = "effect_fireball";
	slot0.name = "spell_fireball";
	Spell& slot2 = data->slot_spell.at(2);
	slot2.icon = "";
	slot2.name = "spell_create_block";
	slot2.tile = 607;
	slot2.type = SpellCursorType::tile;
	Spell& slot3 = data->slot_spell.at(3);
	slot3.icon = "";
	slot3.name = "spell_create_block";
	slot3.tile = 28;
	slot3.type = SpellCursorType::tile;
	Spell& slot4 = data->slot_spell.at(4);
	slot4.icon = "";
	slot4.name = "spell_create_block";
	slot4.tile = 16;
	slot4.type = SpellCursorType::tile;
	Spell& slot9 = data->slot_spell.at(9);
	slot9.icon = "icon_trash_can";
	slot9.name = "spell_clear_block";
	slot9.type = SpellCursorType::tile;
}

Game::~Game() {
	data->gameRegion.SaveRegion();
}

bool Game::IsActive() {
	return data->isActive;
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
	if (w.tile_protected(x,y)) {
		ret << "(protected)";
	}
	return ret.str();
}

void Game::Draw(SDL_Renderer* target) {
	data->drawCollision = Config::getInstance()->getInt("draw_collision");
	double screen_width = globalData.xsize;
	double screen_height = globalData.ysize;
	int screen_boarder = 64;
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
		if (std::string(data->gameRegion.world.tm.layers.at(i).name).find("overlay",0) == std::string::npos ) {
			DrawLayer(target, globalData.spriteHolder.get(), data->gameRegion.world.tm, i, data->topx, data->topy);
		}
	}
	for (size_t i = 0; i < data->gameRegion.world.tm.object_groups.size(); ++i) {
		DrawOjbectGroup(target, data->gameRegion.world.tm, i, data->topx, data->topy);
	}
	if (data->world_mouse_x >= 0 && data->world_mouse_y >= 0) {
		int mousebox_x = data->world_mouse_x - data->world_mouse_x % 32 - data->topx;
		int mousebox_y = data->world_mouse_y - data->world_mouse_y % 32 - data->topy;
		if (data->slot_spell.at(data->slot_selected).type == SpellCursorType::tile) {
			rectangleRGBA(globalData.screen, mousebox_x, mousebox_y,
			              mousebox_x + 32, mousebox_y + 32, 255, 255, 0, 255);
		}
	}
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
			DrawProjectile(target, globalData.spriteHolder.get(), projectile, SDL_GetTicks(), data->topx, data->topy, data->drawCollision);
		}
	}
	for (size_t i = 0; i < data->gameRegion.world.tm.layers.size(); ++i) {
		if (std::string(data->gameRegion.world.tm.layers.at(i).name).find("overlay",0) != std::string::npos ) {
			DrawLayer(target, globalData.spriteHolder.get(), data->gameRegion.world.tm, i, data->topx, data->topy);
		}
	}
	if (data->world_mouse_x >= 0 && data->world_mouse_y >= 0) {
		char buffer[200];
		snprintf(buffer, sizeof(buffer), "world_x = %d, world_y = %d, layer_info:%s",
		         data->world_mouse_x/32, data->world_mouse_y/32,
		         GetLayerInfoForTile(data->gameRegion.world, data->world_mouse_x/32, data->world_mouse_y/32).c_str()
		        );
		data->bottomField.SetText(buffer);
	}
	else {
		data->bottomField.SetText("outside world");
	}
	data->bottomField.Draw(target, 2, screen_height, sago::SagoTextField::Alignment::left, sago::SagoTextField::VerticalAlignment::bottom);
	data->middleField.Draw(target, screen_width/2, screen_height/4, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::bottom);
	DrawRectYellow(target, screen_width-70, screen_height-70, 52, 52);
	DrawTile(target, globalData.spriteHolder.get(), data->gameRegion.world.tm, data->drawTile, screen_width-60, screen_height-60);
	for (size_t i = 0; i < 10; ++i) {
		if (data->slot_selected == i) {
			DrawRectWhite(target, 10+i*56, 10, 52, 52);
		}
		else {
			DrawRectYellow(target, 10+i*56, 10, 52, 52);
		}
		int key_number = (i+1)%10;
		data->number_labels.at(key_number).Draw(target, 10+i*56, 10);
		const Spell& current_spell = data->slot_spell.at(i);
		if (current_spell.icon.length() > 0) {
			globalData.spriteHolder.get()->GetSprite(current_spell.icon).Draw(target, SDL_GetTicks(), 36+i*56, 36);
		}
		if (current_spell.tile > 0) {
			DrawTile(target, globalData.spriteHolder.get(), data->gameRegion.world.tm, current_spell.tile, 20+i*56, 20);
		}
	}
	if (data->consoleActive && data->console) {
		data->console->Draw(target);
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
	if (data->consoleActive && data->console) {
		data->console->ProcessInput(event, processed);
		if (processed) {
			return;
		}
	}
	if (event.type == SDL_KEYDOWN) {
		int tile_x = data->world_mouse_x/32;
		int tile_y = data->world_mouse_y/32;
		if (event.key.keysym.sym == globalData.playerControls.block_create
		        && sago::tiled::tileInBound(data->gameRegion.world.tm, tile_x, tile_y)
		        && !(data->gameRegion.world.tile_protected(tile_x, tile_y)) ) {
			int layer_number = 2; //  Do not hardcode
			uint32_t tile = data->drawTile;
			sago::tiled::setTileOnLayerNumber(data->gameRegion.world.tm, layer_number, tile_x, tile_y, tile);
			data->gameRegion.waterHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
			data->gameRegion.lavaHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
			data->gameRegion.world.init_physics(data->gameRegion.physicsBox);
		}
		if (event.key.keysym.sym == globalData.playerControls.block_delete
		        && sago::tiled::tileInBound(data->gameRegion.world.tm, tile_x, tile_y)
		        && !(data->gameRegion.world.tile_protected(tile_x, tile_y)) ) {
			int layer_number = 2; //  Do not hardcode
			uint32_t tile = 0;
			sago::tiled::setTileOnLayerNumber(data->gameRegion.world.tm, layer_number, tile_x, tile_y, tile);
			data->gameRegion.waterHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
			data->gameRegion.lavaHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
			data->gameRegion.world.init_physics(data->gameRegion.physicsBox);
		}
		if (event.key.keysym.sym == SDLK_1 || event.key.keysym.sym == SDLK_KP_1) {
			data->slot_selected = 0;
		}
		if (event.key.keysym.sym == SDLK_2 || event.key.keysym.sym == SDLK_KP_2) {
			data->slot_selected = 1;
		}
		if (event.key.keysym.sym == SDLK_3 || event.key.keysym.sym == SDLK_KP_3) {
			data->slot_selected = 2;
		}
		if (event.key.keysym.sym == SDLK_4 || event.key.keysym.sym == SDLK_KP_4) {
			data->slot_selected = 3;
		}
		if (event.key.keysym.sym == SDLK_5 || event.key.keysym.sym == SDLK_KP_5) {
			data->slot_selected = 4;
		}
		if (event.key.keysym.sym == SDLK_6 || event.key.keysym.sym == SDLK_KP_6) {
			data->slot_selected = 5;
		}
		if (event.key.keysym.sym == SDLK_7 || event.key.keysym.sym == SDLK_KP_7) {
			data->slot_selected = 6;
		}
		if (event.key.keysym.sym == SDLK_8 || event.key.keysym.sym == SDLK_KP_8) {
			data->slot_selected = 7;
		}
		if (event.key.keysym.sym == SDLK_9 || event.key.keysym.sym == SDLK_KP_9) {
			data->slot_selected = 8;
		}
		if (event.key.keysym.sym == SDLK_0 || event.key.keysym.sym == SDLK_KP_0) {
			data->slot_selected = 9;
		}
		if (event.key.keysym.sym == SDLK_PAGEDOWN) {
			data->drawTile--;
		}
		if (event.key.keysym.sym == SDLK_PAGEUP) {
			data->drawTile++;
		}
		if (event.key.keysym.sym == SDLK_ESCAPE && event.key.keysym.mod & KMOD_LSHIFT) {
			if (!data->console) {
				data->console = std::make_shared<Console>();
			}
			data->console->Activate();
			data->consoleActive = true;
		}
		if (event.key.keysym.sym == SDLK_F12) {
			data->isActive = false;
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
	std::string middleText = "";
	if (data->consoleActive && data->console) {
		data->console->Update();
	}
	Uint32 nowTime = SDL_GetTicks();
	Uint32 deltaTime = nowTime - data->lastUpdate;
	const Uint8* state = SDL_GetKeyboardState(NULL);
	float deltaX = 0.0f;
	float deltaY = 0.0f;
	if (!data->consoleActive) {
		if (state[globalData.playerControls.move_down]) {
			deltaY += 1.0f;
		}
		if (state[globalData.playerControls.move_up]) {
			deltaY -= 1.0f;
		}
		if (state[globalData.playerControls.move_right]) {
			deltaX += 1.0f;
		}
		if (state[globalData.playerControls.move_left]) {
			deltaX -= 1.0f;
		}
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
		MiscItem* item = dynamic_cast<MiscItem*> (entity.get());
		if (item) {
			if (item->destructible && item->health <= 0.0f && !item->removeMe) {
				if (item->body) {
					/*data->gameRegion.physicsBox->DestroyBody(item->body);
					item->body = nullptr;*/
					destroyBodyWithFixtures(data->gameRegion.physicsBox.get(), item->body);
				}
				item->removeMe = true;
			}
		}
	}
	auto& vp = data->gameRegion.placeables;
	size_t preSize = vp.size();
	vp.erase(std::remove_if(vp.begin(), vp.end(), [](std::shared_ptr<Placeable>& p) {
		if (p->removeMe) {
			std::cout << "Found something to remove: "<< p->X << "," << p->Y << "\n" ;
		}
		return p->removeMe;
	}), vp.end());
	if (vp.size() != preSize) {
		std::cout << "Before: " << preSize << ", after: " << vp.size() << "\n";
	}
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
	const std::vector<sago::tiled::TileObjectGroup>& object_groups = data->gameRegion.world.tm.object_groups;
	for (const auto& group : object_groups) {
		for (const auto& item : group.objects) {
			if (item.isEllipse) {
				continue;
			}
			if (item.x < data->human->X && item.y < data->human->Y && item.width+item.x > data->human->X
			        && item.height+item.y > data->human->Y && item.type == "text") {
				const auto& itr = item.properties.find("text");
				if (itr != item.properties.end()) {
					middleText = itr->second.value;
				}
			}
		}
	}
	if (data->human->X < 0) {
		ResetWorld(data->gameRegion.GetRegionX()-1, data->gameRegion.GetRegionY(), false);
		data->human->body->SetTransform(b2Vec2(data->gameRegion.world.tm.width, data->gameRegion.world.tm.height/2),data->human->body->GetAngle()) ;
	}
	if (data->human->X > data->gameRegion.world.tm.width*32) {
		ResetWorld(data->gameRegion.GetRegionX()+1, data->gameRegion.GetRegionY(), false);
		data->human->body->SetTransform(b2Vec2(0.01f, data->gameRegion.world.tm.height/2),data->human->body->GetAngle()) ;
	}
	if (data->human->Y < 0) {
		ResetWorld(data->gameRegion.GetRegionX(), data->gameRegion.GetRegionY()-1, false);
		data->human->body->SetTransform(b2Vec2(data->gameRegion.world.tm.width/2, data->gameRegion.world.tm.height),data->human->body->GetAngle()) ;
	}
	if (data->human->Y > data->gameRegion.world.tm.height*32) {
		ResetWorld(data->gameRegion.GetRegionX(), data->gameRegion.GetRegionY()+1, false);
		data->human->body->SetTransform(b2Vec2(data->gameRegion.world.tm.width/2, 0.01f),data->human->body->GetAngle()) ;
	}
	if (teleport) {
		ResetWorld(teleportX, teleportY, false);
		teleport = false;
	}
	if (openTiled) {
		openTiled = false;
		for (const sago::tiled::TileSet& org_ts : data->gameRegion.world.tm.tileset) {
			const sago::tiled::TileSet* ts = &org_ts;
			while (ts->alternativeSource) {
				ts = ts->alternativeSource;
			}
			std::string source_filename = ts->image.source.substr(3);
			std::string dest_filename = std::string("worlds/")+ data->worldName + "/" + ts->image.source.substr(3);
			std::cout << dest_filename << ":" << ts->image.source << "\n";
			std::string imageFile = sago::GetFileContent(source_filename);
			sago::WriteFileContent(dest_filename.c_str(), imageFile);
		}
		std::string filename = getPathToSaveFiles() + "/" + data->gameRegion.GetFilename();
		std::string command = std::string("tiled \"")+filename+"\"";
		std::system(command.c_str());
		ResetWorldNoSave(data->gameRegion.GetRegionX(), data->gameRegion.GetRegionY(), false);
	}
	if (reset_region) {
		ResetWorld(data->gameRegion.GetRegionX(), data->gameRegion.GetRegionY(), true);
		reset_region = false;
	}
	if (data->consoleActive && data->console && !data->console->IsActive()) {
		data->consoleActive = false;
	}
	if (SDL_GetMouseState(nullptr,nullptr) & 1 && !data->consoleActive) {
		if (data->slot_spell.at(data->slot_selected).name == "spell_fireball") {
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
		if (data->slot_spell.at(data->slot_selected).name == "spell_create_block") {
			if (data->human->castTimeRemaining == 0) {
				data->human->castTimeRemaining = data->human->castTime;
				int tile_x = data->world_mouse_x/32;
				int tile_y = data->world_mouse_y/32;
				int tile = data->slot_spell.at(data->slot_selected).tile;
				if (sago::tiled::tileInBound(data->gameRegion.world.tm, tile_x, tile_y)
				        && !(data->gameRegion.world.tile_protected(tile_x, tile_y)) ) {
					int layer_number = 2; //  Do not hardcode
					sago::tiled::setTileOnLayerNumber(data->gameRegion.world.tm, layer_number, tile_x, tile_y, tile);
					data->gameRegion.waterHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
					data->gameRegion.lavaHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
					data->gameRegion.world.init_physics(data->gameRegion.physicsBox);
				}
			}
		}
		if (data->slot_spell.at(data->slot_selected).name == "spell_clear_block") {
			if (data->human->castTimeRemaining == 0) {
				data->human->castTimeRemaining = data->human->castTime;
				int tile_x = data->world_mouse_x/32;
				int tile_y = data->world_mouse_y/32;
				if (sago::tiled::tileInBound(data->gameRegion.world.tm, tile_x, tile_y)
				        && !(data->gameRegion.world.tile_protected(tile_x, tile_y)) ) {
					int layer_number = 2; //  Do not hardcode
					uint32_t tile = 0;
					sago::tiled::setTileOnLayerNumber(data->gameRegion.world.tm, layer_number, tile_x, tile_y, tile);
					data->gameRegion.waterHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
					data->gameRegion.lavaHandler.updateFirstTile(data->gameRegion.world.tm, tile_x, tile_y);
					data->gameRegion.world.init_physics(data->gameRegion.physicsBox);
				}
			}
		}
	}
	data->middleField.SetText(middleText);
}
