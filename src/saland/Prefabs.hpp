/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2021 Poul Sander

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
https://github.com/salandgame/saland
===========================================================================
*/

#include <string>
#include <vector>
#include "../sagotmx/tmx_struct.h"

void ScanPrefabs(const std::string& filename);

struct Prefab {
	std::string name;

	std::string filename = "";
	int topx = 0;
	int topy = 0;
	int width = 0;
	int height = 0;
};

void ApplyPrefabLayer(sago::tiled::TileMap& dest, int destX, int destY, const char* destLayer, const Prefab& prefab, const char* sourceLayer);

void ApplyPrefabObjectMarker(sago::tiled::TileMap& dest, int destX, int destY, const Prefab& prefab);

/**
 * Does not validate if there are blocking elements underneth.
 * Use GameRegion::SpawnPrefab check that.
 * */
void ApplyPrefab(sago::tiled::TileMap& dest, int destX, int destY, const Prefab& prefab);

Prefab getPrefab(const char* name);

size_t getPrefabCount();

const Prefab& getPrefabByIndex(size_t index);

struct PlacedPrefabInfo {
	std::string name;
	int destX = 0;
	int destY = 0;
	int width = 0;
	int height = 0;
};

/**
 * Looks for a previously placed prefab (tracked via its "prefab_marking" object) whose
 * footprint covers the given tile. Returns false if no placed prefab covers that tile.
 */
bool FindPlacedPrefabAtTile(const sago::tiled::TileMap& tm, int tileX, int tileY, PlacedPrefabInfo& out);

/**
 * Clears the tiles stamped by a previously placed prefab and removes its marking object.
 * Use FindPlacedPrefabAtTile to locate the prefab to remove.
 */
void RemovePlacedPrefab(sago::tiled::TileMap& dest, const PlacedPrefabInfo& info);

