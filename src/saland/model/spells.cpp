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

void SpellHolder::add_spell(const Spell &spell)
{
	spells.push_back(spell);
	std::string name = spell.name;
	if (spell.item_name[0])
	{
		name = name + ":" + spell.item_name;
	}
	if (spell.tile)
	{
		name = name + ":" + std::to_string(spell.tile);
	}
	spellIndex[name] = spells.size() - 1;
}

void SpellHolder::init()
{
	Spell slot0;
	slot0.icon = "effect_fireball";
	slot0.name = "spell_fireball";
	Spell slot1;
	slot1.icon = "item_weapon_long_knife";
	slot1.name = "weapon_slash_long_knife";
	Spell slot2;
	slot2.icon = "";
	slot2.name = "spell_create_block";
	slot2.tile = 607;
	slot2.type = SpellCursorType::tile;
	Spell slot3;
	slot3.icon = "";
	slot3.name = "spell_create_block";
	slot3.tile = 28;
	slot3.type = SpellCursorType::tile;
	Spell slot4;
	slot4.icon = "";
	slot4.name = "spell_create_block";
	slot4.tile = 16;
	slot4.type = SpellCursorType::tile;
	Spell slot5;
	slot5.icon = "item_food_potato";
	slot5.name = "spell_spawn_item";
	slot5.item_name = "food_potato";
	Spell slot6;
	slot6.icon = "item_barrel";
	slot6.name = "spell_spawn_item";
	slot6.item_name = "barrel";
	Spell slot7;
	slot7.icon = "tree_palm"; // Quite big. As this has no icon
	slot7.name = "spell_spawn_item";
	slot7.item_name = "tree_palm";
	Spell slot8;
	slot8.icon = "cactus_one";
	slot8.name = "spell_spawn_item";
	slot8.item_name = "cactus_full";
	Spell slot9;
	slot9.icon = "icon_trash_can";
	slot9.name = "spell_clear_block";
	slot9.type = SpellCursorType::tile;
	add_spell(slot0);
	add_spell(slot1);
	add_spell(slot2);
	add_spell(slot3);
	add_spell(slot4);
	add_spell(slot5);
	add_spell(slot6);
	add_spell(slot7);
	add_spell(slot8);
	add_spell(slot9);
}

size_t SpellHolder::get_spell_count() const
{
	return spells.size();
}

const Spell &SpellHolder::get_spell(size_t index) const
{
	if (index >= spells.size())
	{
		return blankSpell;
	}
	return spells.at(index);
}

const Spell &SpellHolder::get_spell_by_name(const std::string &spell_name) const
{
	auto it = spellIndex.find(spell_name);
	if (it == spellIndex.end())
	{
		return blankSpell;
	}
	return get_spell(it->second);
}