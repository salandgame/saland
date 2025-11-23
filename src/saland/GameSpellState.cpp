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
#define BOX_LINE_LIMIT 16

static int spell_slot_x(int slot) {
	return BOX_OFFSET+(int)(slot%BOX_LINE_LIMIT)*BOX_SPACING;
}

static int spell_slot_y(int slot) {
	return BOX_OFFSET+70+(int)(slot/BOX_LINE_LIMIT)*BOX_SPACING;
}

void GameSpellState::GameSpellState::Draw(SDL_Renderer* target) {
	int sideBoarder = 20;
	sago::SagoLogicalResize* resize = &globalData.logicalResize;
	for (size_t i = 0; i < 10; ++i) {
		if (spell_holder->slot_selected == i) {
			DrawRectWhite(target, BOX_OFFSET+i*BOX_SPACING, BOX_OFFSET, BOX_SIZE, BOX_SIZE, resize);
		}
		else {
			DrawRectYellow(target, BOX_OFFSET+i*BOX_SPACING, BOX_OFFSET, BOX_SIZE, BOX_SIZE, resize);
		}
		int key_number = (i+1)%10;
		data->number_labels.at(key_number).Draw(target, 10+i*BOX_SPACING, 10, sago::SagoTextField::Alignment::left, sago::SagoTextField::VerticalAlignment::top, resize);
		const Spell& current_spell = spell_holder->slot_spell.at(i);
		if (current_spell.icon.length() > 0) {
			globalData.spriteHolder.get()->GetSprite(current_spell.icon).Draw(target, SDL_GetTicks(), 36+i*56, 36, resize);
		}
		if (current_spell.tile > 0) {
			DrawTile(target, globalData.spriteHolder.get(), *tm, current_spell.tile, 20+i*BOX_SPACING, 20, resize);
		}
	}
	if (!data->spellSelectActive) {
		return;
	}
	for (size_t i = 0; i < spell_holder->get_spell_count(); ++i) {
		int x = spell_slot_x(i);
		int y = spell_slot_y(i);
		DrawRectYellow(target, x, y, BOX_SIZE, BOX_SIZE, resize);
		const Spell& current_spell = spell_holder->get_spell(i);
		if (current_spell.icon.length() > 0) {
			globalData.spriteHolder.get()->GetSprite(current_spell.icon).Draw(target, SDL_GetTicks(), x+BOX_SIZE/2, y+BOX_SIZE/2, resize);
		}
		if (current_spell.tile > 0) {
			DrawTile(target, globalData.spriteHolder.get(), *tm, current_spell.tile, x+10, y+10, resize);
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
	if (!data->spellSelectActive) {
		return;
    }
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (event.button.button == SDL_BUTTON_LEFT) {
			// Convert physical mouse coordinates to logical coordinates
			int logical_mousex, logical_mousey;
			globalData.logicalResize.PhysicalToLogical(globalData.mousex, globalData.mousey, logical_mousex, logical_mousey);
			std::cout << "Left click " << logical_mousex << "," << logical_mousey << "\n";
			for (size_t i = 0; i < spell_holder->get_spell_count(); ++i) {
				int x = spell_slot_x(i);
				int y = spell_slot_y(i);
				if (logical_mousex >= x && logical_mousex <= x + BOX_SIZE &&
				        logical_mousey >= y && logical_mousey <= y + BOX_SIZE) {
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
