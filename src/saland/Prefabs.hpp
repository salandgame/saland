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

void ApplyPrefab(sago::tiled::TileMap& dest, int destX, int destY, const Prefab& prefab);

Prefab getPrefab(const char* name);

