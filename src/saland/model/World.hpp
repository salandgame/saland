#ifndef WORLD_HPP
#define WORLD_HPP

#include "../../sagotmx/tmx_struct.h"
#include "../../sago/SagoMisc.hpp"

class World {
public:
	World();
	void init();
//private:
	sago::tiled::TileSet ts;
	sago::tiled::TileMap tm;
};

#endif /* WORLD_HPP */

