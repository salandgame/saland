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
	std::unordered_set<uint32_t> tiles;
	std::map<std::string, uint32_t> tile_map;
	uint32_t default_tile = 28;
	int blockingLayer = -1;
	int blockingLayer_overlay_1 = -1;

	WaterHandler();

	void setupTiles(uint32_t start_tile);

	uint32_t getTile(sago::tiled::TileMap& tm, int x, int y, uint32_t& overlay_tile);

	void updateFirstTile(sago::tiled::TileMap& tm, int x, int y);

	bool isWaterTile(uint32_t tile) const;

	bool isWater(const sago::tiled::TileMap& tm, int x, int y) const;

private:
	void updateTile(sago::tiled::TileMap& tm, int x, int y);

	std::string stringForTileSurrounding(const sago::tiled::TileMap& tm, int x, int y) const;

};

#endif  //TERRAIN_WATERHANDLER_HPP
