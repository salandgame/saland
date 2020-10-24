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

#include "WaterHandler.hpp"
#include <iostream>

WaterHandler::WaterHandler() {
	setupTiles(default_tile);
}

void WaterHandler::setupTiles(uint32_t start_tile) {
	tile_map["11111110"] = 29;
	tile_map["11111011"] = 30;
	tile_map["11011111"] = 61;
	tile_map["01111111"] = 62;

	tile_map["00001011"] = 92;
	tile_map["00011111"] = 93;
	tile_map["00010110"] = 94;

	tile_map["01101011"] = 124;
	tile_map["11111111"] = 125;
	tile_map["11010110"] = 126;

	tile_map["01101000"] = 156;
	tile_map["11111000"] = 157;
	tile_map["11010000"] = 158;

	tiles = {28, 29, 30, 60, 61, 62, 92, 93, 94, 124, 125, 126, 156, 157, 158, 188, 189, 190};
}

uint32_t WaterHandler::getTile(sago::tiled::TileMap& tm, int x, int y) {
	std::string theString = stringForTileSurrounding(tm, x, y);
	uint32_t tile = tile_map[theString];
	if (tile > 0) {
		return tile;
	}
	if (theString[1] == '1' && theString[2] == '1' && theString[4] == '1' && theString[6] == '1' && theString[7] == '1') {
		return tile_map["01101011"];
	}
	if (theString[0] == '1' && theString[1] == '1' && theString[3] == '1' && theString[5] == '1' && theString[6] == '1') {
		return tile_map["11010110"];
	}
	if (theString[3] == '1' && theString[4] == '1' && theString[5] == '1' && theString[6] == '1' && theString[7] == '1') {
		return tile_map["00011111"];
	}
	if (theString[0] == '1' && theString[1] == '1' && theString[2] == '1' && theString[3] == '1' && theString[4] == '1') {
		return tile_map["11111000"];
	}

	if (theString[1] == '1' && theString[2] == '1' && theString[4] == '1') {
		return tile_map["01101000"];
	}
	if (theString[0] == '1' && theString[1] == '1' && theString[3] == '1') {
		return tile_map["11010000"];
	}

	if (theString[3] == '1' && theString[5] == '1' && theString[6] == '1') {
		return tile_map["00010110"];
	}
	if (theString[4] == '1' && theString[6] == '1' && theString[7] == '1') {
		return tile_map["00001011"];
	}
	return default_tile;
}

void WaterHandler::updateTile(sago::tiled::TileMap& tm, int x, int y) {
	if (x < 0 || y < 0 || x > tm.width || y > tm.height) {
		//Out of bound. Do nothing
		return;
	}
	int layer_number = 2; //  Do not hardcode
	uint32_t current_tile = sago::tiled::getTileFromLayer(tm, tm.layers.at(layer_number), x, y);
	std::cout << "tile: " << current_tile << ", surrounding: " << stringForTileSurrounding(tm, x, y) << "\n";
	if (isWaterTile(current_tile)) {
		uint32_t tile = getTile(tm, x, y);
		if (tile != current_tile) {
			sago::tiled::setTileOnLayerNumber(tm, layer_number, x, y, tile);
			updateTile(tm, x-1, y-1);
			updateTile(tm, x, y-1);
			updateTile(tm, x+1, y-1);
			updateTile(tm, x-1, y);
			updateTile(tm, x+1, y);
			updateTile(tm, x-1, y+1);
			updateTile(tm, x, y+1);
			updateTile(tm, x+1, y+1);
		}
	}
}

bool WaterHandler::isWaterTile(uint32_t tile) const {
	return tiles.find(tile) != tiles.end();
}

bool WaterHandler::isWater(const sago::tiled::TileMap& tm, int x, int y) const {
	if (x < 0 || y < 0 || x > tm.width || y > tm.height) {
		//Assume that "water" is around the map
		return true;
	}
	int layer_number = 2; //  Do not hardcode
	uint32_t current_tile = sago::tiled::getTileFromLayer(tm, tm.layers.at(layer_number), x, y);
	return tiles.find(current_tile) != tiles.end();
}

std::string WaterHandler::stringForTileSurrounding(const sago::tiled::TileMap& tm, int x, int y) const {
	std::string ret = std::to_string(isWater(tm, x-1, y-1)) + std::to_string(isWater(tm, x, y-1)) + std::to_string(isWater(tm, x+1, y-1))
	+ std::to_string(isWater(tm, x-1, y)) + std::to_string(isWater(tm, x+1, y))
	+ std::to_string(isWater(tm, x-1, y+1)) + std::to_string(isWater(tm, x, y+1)) + std::to_string(isWater(tm, x+1, y+1));
	return ret; 
}
