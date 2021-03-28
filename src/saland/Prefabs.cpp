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

void ScanPrefabs(const std::string& filename) {
	std::string mapFileName = "maps/"+filename+".tmx";
	std::string tmx_file = sago::GetFileContent(mapFileName);
	sago::tiled::TileMap tm = sago::tiled::string2tilemap(tmx_file);
	for (const auto& t : tm.object_groups) {
		std::cout << "Prefab object group: " << t.name << "\n";
		for (const auto& o : t.objects) {
			Prefab p;
			p.filename = filename;
			p.name = o.name;
			p.topx = o.x/32;
			p.topy = o.y/32;
			std::cout << "Prefab: " << p.name << " " << p.filename << " (" << p.topx << ", " << p.topy << ")\n";
		}
	}
}