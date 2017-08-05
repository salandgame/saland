#ifndef WORLD_HPP
#define WORLD_HPP

#include "../../sagotmx/tmx_struct.h"
#include "../../sago/SagoMisc.hpp"
#include <Box2D/Box2D.h>

class World {
public:
	World();
	void init(std::shared_ptr<b2World>& world);
//private:
	sago::tiled::TileSet ts;
	sago::tiled::TileMap tm;
	std::shared_ptr<b2World> physicsWorld;
};

#endif /* WORLD_HPP */

