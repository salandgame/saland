/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2018 Poul Sander

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

#include "Console.hpp"
#include "../GameDraw.hpp"
#include "../globals.hpp"

Console::Console() {
}

Console::~Console() {
}

bool Console::IsActive() {
	return active;
};
void Console::Draw(SDL_Renderer* target) {
	int sideBoarder = 20;
	DrawRectYellow(target, sideBoarder, 10, globalData.ysize/2, globalData.xsize - sideBoarder*2);
}

void Console::ProcessInput(const SDL_Event& event, bool &processed) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE && event.key.keysym.mod & KMOD_LSHIFT) {
			active = false;
			processed = true;
		}
	}
}

void Console::Update() {
	return;
}
