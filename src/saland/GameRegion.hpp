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

#ifndef GAMEREGION_HPP
#define GAMEREGION_HPP

#include "model/World.hpp"
#include "model/placeables.hpp"
#include "../sagotmx/tmx_struct.h"
#include <vector>
class GameRegion {
public:
	GameRegion();
	void Init(int x, int y);
	std::vector<std::shared_ptr<Placeable> > placeables;
	std::shared_ptr<b2World> physicsBox;
	void SaveRegion();
	World world;
	uint32_t outerTile = 485;
	int GetRegionX() {return region_x; }
	int GetRegionY() {return region_y; }
private:
	int region_x = 0;
	int region_y = 0;
	std::string mapFileName = "maps/sample1.tmx";
};

#endif /* GAMEREGION_HPP */

