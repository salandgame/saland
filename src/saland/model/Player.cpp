/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2024 Poul Sander

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

#include "Player.hpp"

void to_json(nlohmann::json& j, const Player& p) {
	j = nlohmann::json{ {"race", p.race}, {"hair", p.hair}, {"item_inventory", p.item_inventory}, {"equipped_items", p.equipped_items} };
}

void from_json(const nlohmann::json& j, Player& p) {
	j.at("race").get_to(p.race);
	if (j.contains("hair")) {
		j.at("hair").get_to(p.hair);
	}
	j.at("item_inventory").get_to(p.item_inventory);
	if (j.contains("equipped_items")) {
		j.at("equipped_items").get_to(p.equipped_items);
	}
}