/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2026 Poul Sander

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

#include "GameHair.hpp"
#include <iostream>
#include <format>
#include "../sago/SagoMisc.hpp"
#include "rapidjson/document.h"

const char* const hair_dir = "saland/hair/";

static HairDef hairDefDefault;

static std::vector<HairDef> all_hair;

static bool matchesRace(const HairDef& hair, const std::string& race) {
	if (hair.restriction.empty()) {
		return true;
	}
	for (const auto& r : hair.restriction) {
		if (r == race) {
			return true;
		}
	}
	return false;
}

static void ReadHairFile(const std::string& filename) {
	std::string content = sago::GetFileContent(filename.c_str());
	rapidjson::Document document;
	document.Parse(content.c_str());
	if (!document.IsObject()) {
		std::cerr << "Failed to parse: " << filename << "\n";
		return;
	}
	for (auto& m : document.GetObject()) {
		const std::string& header = m.name.GetString();
		if (header == "hair") {
			const auto& items = m.value;
			if (!items.IsArray()) {
				std::cerr << "Failure reading " << filename << ": 'hair' must be an array\n";
				continue;
			}
			for (const auto& item : items.GetArray()) {
				if (item.IsObject()) {
					HairDef new_hair = hairDefDefault;
					for (const auto& member : item.GetObject()) {
						if (member.name == "hairid") {
							new_hair.hairid = member.value.GetString();
						}
						if (member.name == "displayName") {
							new_hair.displayName = member.value.GetString();
						}
						if (member.name == "bg") {
							new_hair.bg = member.value.GetString();
						}
						if (member.name == "restriction") {
							if (member.value.IsArray()) {
								for (const auto& r : member.value.GetArray()) {
									if (r.IsString()) {
										new_hair.restriction.push_back(r.GetString());
									}
								}
							}
						}
					}
					all_hair.push_back(new_hair);
				}
			}
		}
	}
}

static void initHair() {
	std::vector<std::string> file_list = sago::GetFileList(hair_dir);
	for (const std::string& file : file_list) {
		std::string filename = std::format("{}{}", hair_dir, file);
		printf("Hair file: %s\n", filename.c_str());
		ReadHairFile(filename);
	}
}

const HairDef& getHairDef(const std::string& hairName, const std::string& race) {
	if (all_hair.empty()) {
		initHair();
	}
	for (const auto& hair : all_hair) {
		if (hair.hairid == hairName && matchesRace(hair, race)) {
			return hair;
		}
	}
	return hairDefDefault;
}

std::vector<HairDef> getHairOptions(const std::string& race) {
	if (all_hair.empty()) {
		initHair();
	}
	std::vector<HairDef> result;
	for (const auto& hair : all_hair) {
		if (matchesRace(hair, race)) {
			result.push_back(hair);
		}
	}
	return result;
}
