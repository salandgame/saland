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
			Draw(renderer, texture, 32*i-topx, 32*j-topy, part);
		}
	}
}

static void DrawOjbectGroup (SDL_Renderer* renderer, const sago::tiled::TileMap& tm, size_t object_group, int topx, int topy) {
	const sago::tiled::TileObjectGroup& group = tm.object_groups.at(object_group);
	for (const sago::tiled::TileObject& o : group.objects) {
		rectangleRGBA(renderer, o.x - topx, o.y - topy,
		o.x+o.width-topx, o.y + o.height - topy, 255, 255, 0, 255);
	}
}

struct Game::GameImpl {
	std::vector<std::shared_ptr<Placeable> > placeables;
	float time = 0.0;
	std::shared_ptr<Human> human;
	float center_x = 0;
	float center_y = 0;
	bool drawCollision = true;
	World world;
	int topx = 0.0;
	int topy = 0.0;
	int world_mouse_x = 0;  //Mouse cooridinates relative to the world
	int world_mouse_y = 0;
	char direction = 0;
	Uint32 lastUpdate = 0;
};

Game::Game() {
	data.reset(new Game::GameImpl());
	data->world.init();
	data->lastUpdate = SDL_GetTicks();
	data->human.reset(new Human());
}

Game::~Game() {
	std::string data2save = sago::tiled::tilemap2string(data->world.tm);
	sago::WriteFileContent("maps/sample1.tmx", data2save);
}

bool Game::IsActive() {
	return true;
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
		relativeAnimationState = 1.0f-(entity->castTimeRemaining/entity->castTime);
	}
	if (drawCollision || true) {
		circleRGBA(target,
				entity->X-offsetX, entity->Y-offsetY, entity->Radius,
				255, 255, 0, 255);
	}
	const sago::SagoSprite &mySprite = sHolder->GetSprite(entity->race + "_"+animation+"_"+std::string(1,entity->direction));
	if (relativeAnimation) {
		mySprite.DrawProgressive(target, relativeAnimationState, std::round(entity->X)-offsetX, std::round(entity->Y)-offsetY);
	}
	else {
		mySprite.Draw(target, time, std::round(entity->X)-offsetX, std::round(entity->Y)-offsetY);
	}
	/*const sago::SagoSprite &myHair = sHolder->GetSprite(entity->race + "_"+animation+"_hair_1_"+string(1,entity->direction));
	myHair.Draw(target, time, entity->X-offsetX, entity->Y-offsetY);*/
}

void Game::Draw(SDL_Renderer* target) {
	data->topx = std::round(data->center_x - 1024.0/2.0);
	data->topy = std::round(data->center_y - 768.0/2.0);
	if (data->topx < -10) {
		data->topx = -10;
	}
	if (data->topy < -10) {
		data->topy = -10;
	}
	SDL_Texture* texture = globalData.spriteHolder->GetDataHolder().getTexturePtr("terrain");
	for (size_t i = 0; i < data->world.tm.layers.size(); ++i ) {
		DrawLayer(target, texture, data->world.tm, i, data->topx, data->topy);
	}
	for (size_t i = 0; i <data->world.tm.object_groups.size(); ++i) {
		DrawOjbectGroup(target, data->world.tm, i, data->topx, data->topy);
	}
	int mousebox_x = data->world_mouse_x - data->world_mouse_x%32 - data->topx;
	int mousebox_y = data->world_mouse_y - data->world_mouse_y%32 - data->topy;
	rectangleRGBA(globalData.screen, mousebox_x, mousebox_y,
		mousebox_x+32, mousebox_y+32, 255, 255, 0, 255);
	//Draw human
	DrawHumanEntity(target, globalData.spriteHolder.get(), data->human.get(), SDL_GetTicks(), data->topx, data->topy, false);
}

void Game::ProcessInput(const SDL_Event& event, bool& processed) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_SPACE) {
			if (data->human->castTimeRemaining == 0) {
				data->human->castTimeRemaining = data->human->castTime;
			}
			processed = true;
		}
	}
}

static std::vector<std::pair<int, int> > getTouchingTiles(float x, float y, float radius) {
	const int tileSize = 32;
	std::vector<std::pair<int, int> > res;
	std::vector<std::pair<int, int> > candidates;
	for (int i = floor((x-radius) / tileSize); i < (x+radius)/ tileSize ; i++ ) {
		for (int j = floor((y-radius) / tileSize); j < (y+radius)/ tileSize ; j++ ) {
			candidates.push_back(std::make_pair(i,j));
		}
	}
	res = candidates;
	return res;
}

static bool MoveEntity( Placeable* entity, float destX, float destY ) {
	bool canMove = true;
	float sourceX = entity->X;
	float sourceY = entity->Y;
	const auto& touchingTilesNew = getTouchingTiles(destX, destY, entity->Radius);
	for (const auto& tile : touchingTilesNew) {
		
	}
	entity->X = destX;
	entity->Y = destY;
	if ( !canMove ) {
		entity->X = sourceX;
		entity->Y = sourceY;
	}
	return canMove;
}

static void MoveHumanEntity (Creature *entity, float directionX, float directionY, float fDeltaTime) {
	float deltaX = directionX;
	float deltaY = directionY;
	if (deltaX == 0.0f && deltaY == 0.0f) {
		entity->moving = false;
		return;
	}
	entity->moving = true;
	if (deltaX*deltaX+deltaY*deltaY > 1.5f) {
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
	float speed = 4.0f;
	MoveEntity (entity, entity->X + deltaX*(fDeltaTime/speed), entity->Y + deltaY*(fDeltaTime/speed));
}

static void UpdateHuman(Human *entity, float fDeltaTime) {
	if (entity->castTimeRemaining > 0) {
		entity->castTimeRemaining -= fDeltaTime;
	}
	if (entity->castTimeRemaining < 0) {
		entity->castTimeRemaining = 0;
	}
	if (entity->castTimeRemaining == 0) {
		MoveHumanEntity(entity, entity->moveX, entity->moveY, fDeltaTime);
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
	data->center_x = std::round(data->human->X);
	data->center_y = std::round(data->human->Y);
	int mousex;
	int mousey;
	SDL_GetMouseState(&mousex, &mousey);
	data->world_mouse_x = data->topx + mousex;
	data->world_mouse_y = data->topy + mousey;
	std::cout << "world x: " << data->world_mouse_x << ", y: " << data->world_mouse_y << "             \r";
	data->lastUpdate = nowTime;
}
