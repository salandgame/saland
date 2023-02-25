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
#include <unordered_map>
#include "../sago/SagoMisc.hpp"
#include "rapidjson/document.h"
#include "iostream"
#include "fmt/core.h"

const char* const item_dir = "saland/items/";

static ItemDef itemDef;

static std::unordered_map<std::string,ItemDef> all_items;

static void ReadItemFile(const std::string& filename) {
	std::string fullfile = filename;
	std::string content = sago::GetFileContent(fullfile.c_str());
	rapidjson::Document document;
	document.Parse(content.c_str());
	if ( !document.IsObject() ) {
		std::cerr << "Failed to parse: " << fullfile << "\n";
		return;
	}
	for (auto& m : document.GetObject()) {
		const std::string& itemHeader = m.name.GetString();
		if (itemHeader == "items") {
			const auto& items = m.value;
			if (!items.IsArray()) {
				std::cerr << "Failure reading " << filename <<  ": 'items' must be an array" << "\n";
			}
			for (const auto& item : items.GetArray()) {
				if (item.IsObject()) {
					ItemDef new_item = itemDef;
					for (const auto& member : item.GetObject()) {
						if (member.name == "itemid") {
							new_item.itemid = member.value.GetString();
						}
						if (member.name == "sprite") {
							new_item.sprite = member.value.GetString();
						}
						if (member.name == "radius") {
							new_item.radius = member.value.GetDouble();
						}
						if (member.name == "sprite2") {
							new_item.sprite2 = member.value.GetString();
						}
						if (member.name == "isStatic") {
							new_item.isStatic = member.value.GetBool();
						}
						if (member.name == "isDestructible") {
							new_item.isDestructible = member.value.GetBool();
						}
						if (member.name == "health") {
							new_item.health = member.value.GetDouble();
						}
						if (member.name == "pickup") {
							new_item.pickup = member.value.GetBool();
						}
					}
					all_items[new_item.itemid] = new_item;
				}
			}
		}
		if (!m.value.IsArray()) {
			if (itemHeader[0] != '_') {
				std::cerr << "Missing top array: " << itemHeader << "\n";
			}
			continue;
		}
	}
}

static void initItems() {
	itemDef.sprite2 = "";
	itemDef.isDestructible = true;
	itemDef.health = 100.0f;
	itemDef.itemid = "";
	itemDef.pickup = false;
	{
		ItemDef barrel = itemDef;
		barrel.itemid = "barrel";
		barrel.radius = 16.0f;
		barrel.sprite = "item_barrel";
		barrel.isStatic = true;
		all_items[barrel.itemid] = barrel;
	}
	{
		ItemDef pine = itemDef;
		pine.itemid = "tree_pine";
		pine.radius = 20.0f;
		pine.sprite = "tree_pine_trunk";
		pine.sprite2 = "tree_pine_top";
		pine.isStatic = true;
		all_items[pine.itemid] = pine;
	}
	{
		ItemDef palm = itemDef;
		palm.itemid = "tree_palm";
		palm.radius = 20.0f;
		palm.sprite = "tree_palm";
		palm.isStatic = true;
		all_items[palm.itemid] = palm;
	}
	{
		ItemDef cactus = itemDef;
		cactus.itemid = "cactus_full";
		cactus.radius = 20.0f;
		cactus.sprite = "cactus_full";
		cactus.isStatic = true;
		all_items[cactus.itemid] = cactus;
	}
	std::vector<std::string> file_list = sago::GetFileList(item_dir);
	for(const std::string& file : file_list) {
		std::string filename = fmt::format("{}{}", item_dir, file);
		printf("Item file: %s\n",filename.c_str());
		ReadItemFile(filename);
	}
}

const ItemDef& getItem(const std::string& itemName) {
	if (all_items.size() == 0) {
		initItems();
	}
	return all_items[itemName];
}