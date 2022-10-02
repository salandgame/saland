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

#include "GameItems.hpp"

static ItemDef itemDef;

const ItemDef& getItem(const std::string& itemName) {
	itemDef.sprite2 = "";
	itemDef.isDestructible = true;
	itemDef.health = 100.0f;
	itemDef.itemid = itemName;
	itemDef.pickup = false;
	if (itemName == "barrel") {
		itemDef.radius = 16.0f;
		itemDef.sprite = "item_barrel";
		itemDef.isStatic = true;
	}
	if (itemName == "food_potato") {
		itemDef.isDestructible = false;
		itemDef.radius = 9.0f;
		itemDef.sprite = "item_food_potato";
		itemDef.isStatic = false;
		itemDef.pickup = true;
	}
	if (itemName == "tree_pine") {
		itemDef.radius = 20.0f;
		itemDef.sprite = "tree_pine_trunk";
		itemDef.sprite2 = "tree_pine_top";
		itemDef.isStatic = true;
	}
	if (itemName == "tree_palm") {
		itemDef.radius = 20.0f;
		itemDef.sprite = "tree_palm";
		itemDef.isStatic = true;
	}
	return itemDef;
}