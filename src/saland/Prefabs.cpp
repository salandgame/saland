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

#include "Prefabs.hpp"
#include "../sago/SagoMisc.hpp"
#include "../sagotmx/tmx_struct.h"


std::vector<Prefab> prefabs;
std::map<std::string, sago::tiled::TileMap> prefabTileMaps;
std::map<std::string, size_t> prefabs_map;

static int GetLayerNumber(const sago::tiled::TileMap& tm, const char* name) {
	for (size_t i=0; i < tm.layers.size(); ++i) {
		if (tm.layers[i].name == name) {
			return i;
		}
	}
	std::cerr << "Failed to find layer: " << name << "\n";
	return -1;
}

static int32_t translate_tile(const sago::tiled::TileMap& dest, const sago::tiled::TileMap& source, int32_t source_tile) {
	if (source_tile == 0) {
		return 0;
	}
	for (size_t i = 0; i < source.tileset.size(); ++i) {
		const sago::tiled::TileSet ts = source.tileset.at(i);
		std::cerr << "Tile: " << source_tile << " between " << ts.firstgid << " and " << 1024 << "?\n";
		if (ts.firstgid <= source_tile && ts.firstgid+1024 > source_tile) {
			std::cerr << "Looking at: " << ts.source << "\n";
			for (size_t j = 0; j < dest.tileset.size(); ++j) {
				const sago::tiled::TileSet ts2 = dest.tileset.at(j);
				if (ts.source == ts2.source) {
					return source_tile - ts.firstgid + ts2.firstgid;
				}
			}
			break;
		}
	}
	return 1;
}

void ApplyPrefabLayer(sago::tiled::TileMap& dest, int destX, int destY, const char* destLayer, const Prefab& prefab, const char* sourceLayer) {
	const sago::tiled::TileMap& source = prefabTileMaps[prefab.filename];
	int destLayerNumber = GetLayerNumber(dest, destLayer);
	int sourceLayerNumber = GetLayerNumber(source, sourceLayer);
	if (destLayerNumber < 0 || sourceLayerNumber < 0) {
		return;
	}
	for (int i = 0; i < prefab.width; ++i) {
		for (int j = 0; j < prefab.height; ++j) {
			int32_t tile = sago::tiled::getTileFromLayer(source, source.layers.at(sourceLayerNumber), prefab.topx+i, prefab.topy+j);
			tile = translate_tile(dest, source, tile);
			sago::tiled::setTileOnLayerNumber(dest, destLayerNumber, destX+i, destY+j, tile);
		}
	}
}

void ApplyPrefabObjectMarker(sago::tiled::TileMap& dest, int destX, int destY, const Prefab& prefab) {
	std::vector<sago::tiled::TileObjectGroup>& object_groups = dest.object_groups;
	for (auto& group : object_groups) {
		if (group.name == "prefab_marking") {
			sago::tiled::TileObject o;
			o.name = prefab.name;
			o.x = destX*32+2;
			o.y = destY*32+2;
			o.width = prefab.width*32-4;
			o.height = prefab.height*32-4;
			group.objects.push_back(o);
		}
	}
}


void ApplyPrefab(sago::tiled::TileMap& dest, int destX, int destY, const Prefab& prefab) {
	if (prefab.height < 1) {
		return;
	}
	ApplyPrefabLayer(dest, destX, destY, "prefab_ground_1", prefab, "prefab_ground_1");
	ApplyPrefabLayer(dest, destX, destY, "blocking", prefab, "prefab_blocking_1");
	ApplyPrefabLayer(dest, destX, destY, "prefab_blocking_2", prefab, "prefab_blocking_2");
	ApplyPrefabLayer(dest, destX, destY, "prefab_overlay_1", prefab, "prefab_overlay_1");
	ApplyPrefabObjectMarker(dest, destX, destY, prefab);
}

Prefab getPrefab(const char* name) {
	Prefab p;
	auto it = prefabs_map.find(name);
	if (it == prefabs_map.end()) {
		return p;
	}
	size_t id = it->second;
	if (id >= prefabs.size()) {
		return p;
	}
	p = prefabs.at(id);
	return p;
}

void TestApplyPrefab(sago::tiled::TileMap& dest, int destX, int destY) {
	if (prefabs.size() == 0) {
		return;
	}
	ApplyPrefab(dest, destX, destY, prefabs.front());
}

void ScanPrefabs(const std::string& filename) {
	std::string mapFileName = "maps/"+filename+".tmx";
	std::string tmx_file = sago::GetFileContent(mapFileName);
	sago::tiled::TileMap tm = sago::tiled::string2tilemap(tmx_file);
	prefabTileMaps[filename] = tm;
	for (const auto& t : tm.object_groups) {
		std::cout << "Prefab object group: " << t.name << "\n";
		for (const auto& o : t.objects) {
			Prefab p;
			p.filename = filename;
			p.name = o.name;
			p.topx = o.x/32;
			p.topy = o.y/32;
			p.width = (o.x+o.width)/32+1 - p.topx;
			p.height = (o.y+o.height)/32+1 - p.topy;
			std::cout << "Prefab: " << p.name << " " << p.filename << " (" << p.topx << ", " << p.topy << ", " << p.width << ", " << p.height << ")\n";
			prefabs.push_back(p);
			prefabs_map[p.name] = prefabs.size()-1;
		}
	}
}