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

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "../sago/SagoSpriteHolder.hpp"
#include <memory>
#include "model/Player.hpp"

struct PlayerControls {
	SDL_Scancode move_up = SDL_SCANCODE_W;
	SDL_Scancode move_down = SDL_SCANCODE_S;
	SDL_Scancode move_left = SDL_SCANCODE_A;
	SDL_Scancode move_right = SDL_SCANCODE_D;
	SDL_Keycode block_create = SDLK_q;
	SDL_Keycode block_delete = SDLK_e;
};


struct GlobalData {
	bool isShuttingDown = false;
	bool fullscreen = false;
	bool resetVideo = false;
	bool SoundEnabled = true;
	bool NoSound = false;
	SDL_Renderer* screen = nullptr;
	std::unique_ptr<sago::SagoSpriteHolder> spriteHolder;
	sago::SagoDataHolder* dataHolder;
	int verboseLevel = 0;
	int mousex = 0;
	int mousey = 0;
	int xsize = 1024;
	int ysize = 768;
	PlayerControls playerControls;
	Player player;

	bool debugDrawCollision = false;
	bool debugDrawProtectedAreas = false;
};

extern GlobalData globalData;

#endif /* GLOBALS_HPP */

