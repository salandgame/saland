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

#ifndef MODEL_SPELLS_HPP
#define MODEL_SPELLS_HPP

#include <string>
#include <vector>
#include <array>
#include <map>

enum class SpellCursorType { dot = 0, tile=1, shpere = 2 };

struct Spell {
	std::string name;
	std::string icon;
	std::string item_name;
	int tile = 0;  //Number reference to a tile, we might also need a tileset at some point
	SpellCursorType type = SpellCursorType::dot;
};

class SpellHolder {
	std::vector<Spell> spells;
	std::map<std::string, size_t> spellIndex;
	Spell blankSpell;
	Spell clearTileSpell;
	bool initialized = false;
public:
	size_t slot_selected = 0;
	std::array<Spell, 10> slot_spell;
	// methods starts here
	void init();
	void add_spell(const Spell& spell);
	size_t get_spell_count() const;
	const Spell& get_spell(size_t index) const;
	const Spell& get_spell_by_name(const std::string&) const;
	const Spell& get_spell_clear_tile() const;
	void ReadSpellFile(const std::string& filename);
};

#endif  //MODEL_SPELLS_HPP