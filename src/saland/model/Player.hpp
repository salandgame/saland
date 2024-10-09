/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2019 Poul Sander

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

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <map>
#include "nlohmann/json.hpp"

struct Player {
	Player() {};
	std::string save_name="player1";
	std::string race = "female";
	std::string get_visible_race() {
		return this->race;
	}
	std::string get_visible_hair() {
		return "standard_hair";
	}
	std::string get_visible_bottom() {
		return "pants_1";
	}
	std::string get_visible_top() {
		if (race == "male") {
			return "";
		}
		return "pirate_shirt_sky";
	}
	std::string get_visibla_hat() {
		return "";
	}
	std::string get_visible_shoes() {
		return "";
	}
	std::map<std::string, int> item_inventory;
};

void to_json(nlohmann::json& j, const Player& p);
void from_json(const nlohmann::json& j, Player& p);

#endif  //PLAYER_HPP