/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2020 Poul Sander

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

#ifndef TERRAIN_WATERHANDLER_HPP
#define TERRAIN_WATERHANDLER_HPP

#include <unordered_set>
#include "../sagotmx/tmx_struct.h"

struct WaterHandler {
	std::unordered_set<uint32_t> tiles = {28, 29, 30, 60, 61, 62, 92, 93, 94, 188, 189, 190};
	uint32_t default_tile = 28;

	void updateTile(sago::tiled::TileMap& world, int x, int y);
};

#endif  //TERRAIN_WATERHANDLER_HPP
