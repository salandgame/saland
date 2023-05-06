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

#include "spells.hpp"
#include "../../sago/SagoMisc.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include "fmt/core.h"

void SpellHolder::add_spell(const Spell& spell) {
	spells.push_back(spell);
	std::string name = spell.name;
	if (spell.item_name[0]) {
		name = name + ":" + spell.item_name;
	}
	if (spell.tile) {
		name = name + ":" + std::to_string(spell.tile);
	}
	spellIndex[name] = spells.size() - 1;
}

void SpellHolder::ReadSpellFile(const std::string& filename) {
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
		if (itemHeader == "spells") {
			const auto& spells = m.value;
			if (!spells.IsArray()) {
				std::cerr << "Failure reading " << filename <<  ": 'items' must be an array" << "\n";
			}
			for (const auto& spell_data : spells.GetArray()) {
				if (spell_data.IsObject()) {
					Spell spell = blankSpell;
					for (const auto& member : spell_data.GetObject()) {
						if (member.name == "name") {
							spell.name = member.value.GetString();
						}
						if (member.name == "icon") {
							spell.icon = member.value.GetString();
						}
						if (member.name == "item_name") {
							spell.item_name = member.value.GetString();
						}
						if (member.name == "tile") {
							spell.tile = member.value.GetInt64();
						}
						if (member.name == "type") {
							std::string type_value = member.value.GetString();
							if (type_value == "dot") {
								spell.type = SpellCursorType::dot;
							}
							else if (type_value == "tile") {
								spell.type = SpellCursorType::tile;
							}
							else {
								std::cerr << filename << ": unsupported tile type: " << type_value << "\n";
							}
						}
					}
					add_spell(spell);
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

void SpellHolder::init() {
	clearTileSpell.icon = "icon_trash_can";
	clearTileSpell.name = "spell_clear_block";
	clearTileSpell.type = SpellCursorType::tile;
	const char* spellDir = "saland/spells/";
	std::vector<std::string> file_list = sago::GetFileList(spellDir);
	for(const std::string& file : file_list) {
		std::string filename = fmt::format("{}{}", spellDir, file);
		printf("Spells: file: %s\n",filename.c_str());
		ReadSpellFile(filename);
	}
}

size_t SpellHolder::get_spell_count() const {
	return spells.size();
}

const Spell& SpellHolder::get_spell(size_t index) const {
	if (index >= spells.size()) {
		return blankSpell;
	}
	return spells.at(index);
}

const Spell& SpellHolder::get_spell_by_name(const std::string& spell_name) const {
	auto it = spellIndex.find(spell_name);
	if (it == spellIndex.end()) {
		return blankSpell;
	}
	return get_spell(it->second);
}

const Spell& SpellHolder::get_spell_clear_tile() const {
	return clearTileSpell;
}
