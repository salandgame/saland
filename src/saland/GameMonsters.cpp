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

#include "GameMonsters.hpp"
#include <unordered_map>
#include "../sago/SagoMisc.hpp"
#include "rapidjson/document.h"
#include <iostream>

static std::unordered_map<std::string, MonsterDef> all_monsters;

static void LoadMonsterDefinitions() {
	if (!all_monsters.empty()) {
		return; // Already loaded
	}

	std::string filename = "monsters/monsters.json";
	std::string content = sago::GetFileContent(filename.c_str());
	rapidjson::Document document;
	document.Parse(content.c_str());

	if (!document.IsObject()) {
		std::cerr << "Failed to parse: " << filename << "\n";
		return;
	}

	for (auto& m : document.GetObject()) {
		const std::string& header = m.name.GetString();
		if (header == "monsters") {
			const auto& monsters = m.value;
			if (!monsters.IsArray()) {
				std::cerr << "Failure reading " << filename << ": 'monsters' must be an array\n";
				continue;
			}

			for (const auto& monster : monsters.GetArray()) {
				if (!monster.IsObject()) {
					continue;
				}

				MonsterDef def;
				std::string race;
				for (const auto& member : monster.GetObject()) {
					std::string memberName = member.name.GetString();

					if (memberName == "race") {
						race = member.value.GetString();
						def.race = race;
					}
					else if (memberName == "radius") {
						def.radius = member.value.GetFloat();
					}
					else if (memberName == "health") {
						def.health = member.value.GetFloat();
					}
					else if (memberName == "speed") {
						def.speed = member.value.GetFloat();
					}
					else if (memberName == "attack" && member.value.IsObject()) {
						for (const auto& attackMember : member.value.GetObject()) {
							std::string attackName = attackMember.name.GetString();
							if (attackName == "cooldownDuration") {
								def.attack.cooldownDuration = attackMember.value.GetFloat();
							}
							else if (attackName == "range") {
								def.attack.range = attackMember.value.GetFloat();
							}
							else if (attackName == "damage") {
								def.attack.damage = attackMember.value.GetFloat();
							}
							else if (attackName == "animation") {
								def.attack.animation = attackMember.value.GetString();
							}
							else if (attackName == "animationDuration") {
								def.attack.animationDuration = attackMember.value.GetFloat();
							}
						}
					}
				}

				if (!race.empty()) {
					all_monsters[race] = def;
				}
			}
		}
	}
}

MonsterDef GetMonsterDefByRace(const std::string& race) {
	LoadMonsterDefinitions();

	auto it = all_monsters.find(race);
	if (it != all_monsters.end()) {
		return it->second;
	}

	// Return default if not found
	std::cerr << "Warning: Monster race '" << race << "' not found in monsters.json\n";
	MonsterDef def;
	def.race = race;
	return def;
}
