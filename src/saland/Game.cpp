#include "Game.hpp"
#include "../sagotmx/tmx_struct.h"
#include "../sago/SagoMisc.hpp"
#include "globals.hpp"
#include "model/placeables.hpp"
#include "SDL.h"

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

struct Game::GameImpl {
	std::vector<std::shared_ptr<Placeable> > placeables;
	float time = 0.0;
	std::shared_ptr<Human> human;
	int center_x = 0;
	int center_y = 0;
	bool drawCollision = true;
	sago::tiled::TileSet ts;
	sago::tiled::TileMap tm;
	float topx = 0.0;
	float topy = 0.0;
	char direction = 0;
	Uint32 lastUpdate = 0;
};

Game::Game() {
	data.reset(new Game::GameImpl());
	std::string tsx_file = sago::GetFileContent("maps/terrain.tsx");
	std::string tmx_file = sago::GetFileContent("maps/sample1.tmx");
	data->ts = sago::tiled::string2tileset(tsx_file);
	data->tm = sago::tiled::string2tilemap(tmx_file);
	data->tm.tileset.alternativeSource = &data->ts;
	data->lastUpdate = SDL_GetTicks();
}

Game::~Game() {
}

bool Game::IsActive() {
	return true;
}

void Game::Draw(SDL_Renderer* target) {
	SDL_Texture* texture = globalData.spriteHolder->GetDataHolder().getTexturePtr("terrain");
	for (size_t i = 0; i < data->tm.layers.size(); ++i ) {
		DrawLayer(target, texture, data->tm, i, data->topx, data->topy);
	}
}

void Game::ProcessInput(const SDL_Event& event, bool& processed) {
}

void Game::Update() {
	Uint32 nowTime = SDL_GetTicks();
	Uint32 deltaTime = nowTime - data->lastUpdate;
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_DOWN]) {
		data->topy += deltaTime/10.0f;
	}
	if (state[SDL_SCANCODE_UP]) {
		data->topy -= deltaTime/10.0f;
	}
	if (state[SDL_SCANCODE_RIGHT]) {
		data->topx += deltaTime/10.0f;
	}
	if (state[SDL_SCANCODE_LEFT]) {
		data->topx -= deltaTime/10.0f;
	}
	
	data->lastUpdate = nowTime;
}
