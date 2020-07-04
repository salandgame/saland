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
    if (itemName == "barrel") {
        itemDef.radius = 16.0f;
        itemDef.sprite = "item_barrel";
        itemDef.itemid = "barrel";
        itemDef.isStatic = true;
    }
    if (itemName == "food_potato") {
        itemDef.radius = 9.0f;
        itemDef.sprite = "item_food_potato";
        itemDef.isStatic = false;
    }
    return itemDef;
}