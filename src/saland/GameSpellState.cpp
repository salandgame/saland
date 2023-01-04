/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2022 Poul Sander

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

#include "GameSpellState.hpp"
#include "GameDraw.hpp"
#include "globals.hpp"
#include "../global.hpp"
#include "../sago/SagoTextField.hpp"
#include <array>

struct GameSpellState::GameSpellStateImpl {
	bool active = true;
	bool spellSelectActive = false;
	std::array<sago::SagoTextField,10> number_labels;  //Label: 0,1...10
};

GameSpellState::GameSpellState() {
	data.reset(new GameSpellState::GameSpellStateImpl());
	for (size_t i = 0; i < data->number_labels.size(); ++i) {
		data->number_labels.at(i).SetHolder(globalData.dataHolder);
		data->number_labels.at(i).SetFontSize(20);
		data->number_labels.at(i).SetText(std::to_string(i));
	}
}

#define BOX_SPACING 56
#define BOX_OFFSET 10
#define BOX_SIZE 52

void GameSpellState::GameSpellState::Draw(SDL_Renderer* target) {
	int sideBoarder = 20;
	for (size_t i = 0; i < 10; ++i) {
		if (spell_holder->slot_selected == i) {
			DrawRectWhite(target, BOX_OFFSET+i*BOX_SPACING, BOX_OFFSET, BOX_SIZE, BOX_SIZE);
		}
		else {
			DrawRectYellow(target, BOX_OFFSET+i*BOX_SPACING, BOX_OFFSET, BOX_SIZE, BOX_SIZE);
		}
		int key_number = (i+1)%10;
		data->number_labels.at(key_number).Draw(target, 10+i*BOX_SPACING, 10);
		const Spell& current_spell = spell_holder->slot_spell.at(i);
		if (current_spell.icon.length() > 0) {
			globalData.spriteHolder.get()->GetSprite(current_spell.icon).Draw(target, SDL_GetTicks(), 36+i*56, 36);
		}
		if (current_spell.tile > 0) {
			DrawTile(target, globalData.spriteHolder.get(), *tm, current_spell.tile, 20+i*BOX_SPACING, 20);
		}
	}
	if (!data->spellSelectActive) {
		return;
	}
	for (size_t i = 0; i < spell_holder->get_spell_count(); ++i) {
		DrawRectYellow(target, BOX_OFFSET+(int)i*BOX_SPACING, BOX_OFFSET+70, BOX_SIZE, BOX_SIZE);
		const Spell& current_spell = spell_holder->get_spell(i);
		if (current_spell.icon.length() > 0) {
			globalData.spriteHolder.get()->GetSprite(current_spell.icon).Draw(target, SDL_GetTicks(), 36+(int)i*BOX_SPACING, 36+70);
		}
		if (current_spell.tile > 0) {
			DrawTile(target, globalData.spriteHolder.get(), *tm, current_spell.tile, 20+(int)i*BOX_SPACING, 20+70);
		}
	}
}

bool GameSpellState::IsActive() {
	return data->active;
};

bool GameSpellState::IsSpellSelectActive() {
	return data->spellSelectActive;
}

void GameSpellState::ProcessInput(const SDL_Event& event, bool& processed) {
	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			data->spellSelectActive = !data->spellSelectActive;
			processed = true;
		}
	}
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (event.button.button == SDL_BUTTON_LEFT) {
			std::cout << "Left click " << globalData.mousex << "," << globalData.mousey << "\n";
			for (size_t i = 0; i < spell_holder->get_spell_count(); ++i) {
				if (globalData.mousex >= BOX_OFFSET+(int)i*BOX_SPACING && globalData.mousex <= BOX_OFFSET+(int)i*BOX_SPACING+BOX_SIZE &&
				        globalData.mousey >= BOX_OFFSET+70 && globalData.mousey <= BOX_OFFSET+70+BOX_SIZE) {
					std::cout << "Clicked: " << i << "\n";
					if (spell_holder->slot_selected < 9) {
						//We do not update slot 9.
						spell_holder->slot_spell.at(spell_holder->slot_selected) = spell_holder->get_spell(i);
					}
				}
			}
		}
	}
};

void GameSpellState::Update() {};

GameSpellState::~GameSpellState() {
}