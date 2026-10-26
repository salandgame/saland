/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2021 Poul Sander

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
https://github.com/salandgame/saland
===========================================================================
*/

#include "PrefabPreview.hpp"
#include "Prefabs.hpp"
#include "GameDraw.hpp"
#include "globals.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

static const int TILE_SIZE = 32;
static const char* PREFAB_ALL = "all";

static std::vector<Prefab> SelectPrefabs(const std::string& name) {
	std::vector<Prefab> selected;
	for (size_t i = 0; i < getPrefabCount(); ++i) {
		const Prefab& prefab = getPrefabByIndex(i);
		if (name == PREFAB_ALL || prefab.name == name) {
			selected.push_back(prefab);
		}
	}
	return selected;
}

void ListPrefabs() {
	for (size_t i = 0; i < getPrefabCount(); ++i) {
		const Prefab& prefab = getPrefabByIndex(i);
		std::cout << prefab.name << " " << prefab.width << "x" << prefab.height << " tiles (" << prefab.filename << ")\n";
	}
}

static bool SavePrefabPng(SDL_Renderer* renderer, const Prefab& prefab, const std::string& outputPath) {
	int width = prefab.width * TILE_SIZE;
	int height = prefab.height * TILE_SIZE;
	SDL_Texture* target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
	if (!target) {
		std::cerr << "Could not create render target for " << prefab.name << ": " << SDL_GetError() << "\n";
		return false;
	}
	SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, target);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	DrawPrefab(renderer, globalData.spriteHolder.get(), prefab, 0, 0, PREFAB_DRAW_OPAQUE);
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
	bool ok = surface != nullptr;
	if (ok && SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
		std::cerr << "Could not read pixels for " << prefab.name << ": " << SDL_GetError() << "\n";
		ok = false;
	}
	SDL_SetRenderTarget(renderer, nullptr);
	if (ok && IMG_SavePNG(surface, outputPath.c_str()) != 0) {
		std::cerr << "Could not write " << outputPath << ": " << SDL_GetError() << "\n";
		ok = false;
	}
	if (surface) {
		SDL_FreeSurface(surface);
	}
	SDL_DestroyTexture(target);
	return ok;
}

int RenderPrefabsToPng(const std::string& name, const std::string& outputDir) {
	std::vector<Prefab> selected = SelectPrefabs(name);
	if (selected.size() == 0) {
		std::cerr << "No prefab named \"" << name << "\". Use --list-prefabs to see the available names.\n";
		return 1;
	}
	int exitCode = 0;
	for (const Prefab& prefab : selected) {
		std::string outputPath = outputDir + "/prefab_" + prefab.name + ".png";
		if (!SavePrefabPng(globalData.screen, prefab, outputPath)) {
			exitCode = 1;
			continue;
		}
		std::cout << "Wrote " << outputPath << "\n";
	}
	return exitCode;
}
