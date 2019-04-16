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

#ifndef WORLD_HPP
#define WORLD_HPP

#include "../../sagotmx/tmx_struct.h"
#include "../../sago/SagoMisc.hpp"
#include <Box2D/Box2D.h>

class World {
public:
	World();
	void init(std::shared_ptr<b2World>& world);
	void init(std::shared_ptr<b2World>& world, const std::string& mapFileName);
	void init_physics(std::shared_ptr<b2World>& world);
	bool tile_protected(int x, int y) const;
//private:
	sago::tiled::TileSet ts;
	sago::tiled::TileMap tm;
	std::shared_ptr<b2World> physicsWorld;
	std::vector<b2Body*> managed_bodies;
	std::vector<bool> protected_tiles;
};

#endif /* WORLD_HPP */

