#ifndef GAME_HPP
#define GAME_HPP

#include "../sago/GameStateInterface.hpp"
#include "../sagotmx/tmx_struct.h"

class Game : public sago::GameStateInterface {
public:
	Game();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool &processed) override;
	virtual void Update() override;
	virtual ~Game();
private:
	sago::tiled::TileSet ts;
	sago::tiled::TileMap tm;
};

#endif /* GAME_HPP */

