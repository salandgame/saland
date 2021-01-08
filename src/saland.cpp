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

#include <iostream>
#include <boost/program_options.hpp>
#include <SDL_mixer.h>
#include "sago/SagoDataHolder.hpp"
#include "sago/SagoSpriteHolder.hpp"
#include "sago/GameStateInterface.hpp"
#include "sago/SagoTextField.hpp"
#include <sstream>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include "saland/globals.hpp"
#include "saland/Game.hpp"

#include "sago/SagoMisc.hpp"
#include "sago/platform_folders.h"
#include "sagotmx/tmx_struct.h"
#include "sago/SagoTextBox.hpp"

#include "MenuSystem.h"

#include "common.h"
#include "os.hpp"
#include "version.h"

//if SHAREDIR is not used we look in current directory
#ifndef SHAREDIR
#define SHAREDIR "."
#endif

#ifndef LOCALEDIR
#define LOCALEDIR SHAREDIR"/locale"
#endif

#ifndef PACKAGE
#define PACKAGE "salandgame"
#endif


GlobalData globalData;

class TitleScreen : public sago::GameStateInterface {
public:
	TitleScreen() {
		textField.SetHolder(globalData.dataHolder);
		textField.SetFontSize(30);
		textBox.SetHolder(globalData.dataHolder);
		textBox.SetFontSize(16);
		testBox.SetHolder(globalData.dataHolder);
		testBox.SetFontSize(16);
	}

	virtual bool IsActive() override {
		return isActive;
	}

	virtual void Draw(SDL_Renderer* target) override {
		textField.SetText("Saland Adventures - The game that has a very long subtitle to test the outline");
		textField.SetOutline(3, SDL_Color{255,165,0,255});
		textField.Draw(target, 10, 10);
		textBox.SetText("This is an attempt to create 2d rpg with a modifiable world.\nLet's see how it goes!");
		textBox.SetOutline(1, SDL_Color{255,0,0,255});
		textBox.SetMaxWidth(800);
		textBox.Draw(target, 300,300);
		testBox.SetText("a\xe6\x97\xa5\xd1\x88\xc3\x86");
		testBox.SetMaxWidth(1);
		testBox.Draw(target, 300,500);
		circleRGBA(target,
		           150, 150, 75,
		           0, 0, 255, 255);
		const sago::SagoSprite& s = globalData.spriteHolder->GetSprite("male_walkcycle_E");
		s.Draw(globalData.screen, SDL_GetTicks(), 100, 100);
		const sago::SagoSprite& sc = globalData.spriteHolder->GetSprite("female_spellcast_E");
		sc.Draw(globalData.screen, SDL_GetTicks(), 200, 100);
		const sago::SagoSprite& bee = globalData.spriteHolder->GetSprite("bee_E");
		bee.Draw(globalData.screen, SDL_GetTicks(), 100, 200);
		const sago::SagoSprite& bat = globalData.spriteHolder->GetSprite("bat_W");
		bat.Draw(globalData.screen, SDL_GetTicks(), 200, 200);
		const sago::SagoSprite& potato = globalData.spriteHolder->GetSprite("item_food_potato");
		potato.Draw(globalData.screen, SDL_GetTicks(), 240, 200);
	}

	virtual void ProcessInput(const SDL_Event& event, bool& processed) override {
		if ( event.type == SDL_KEYDOWN ) {
			if (event.key.keysym.sym == SDLK_RETURN) {
				isActive = false;
				processed = true;
			}
		}
	}

	virtual void Update() override {

	}
private:
	bool isActive = true;
	sago::SagoTextField textField;
	sago::SagoTextBox textBox;
	sago::SagoTextBox testBox;
};

/**
 * This function reads the mouse coordinates from a relevant event.
 * Unlike SDL_GetMouseState this works even if SDL_RenderSetLogicalSize is used
 * @param event
 * @param mousex
 * @param mousey
 */
void UpdateMouseCoordinates(const SDL_Event& event, int& mousex, int& mousey) {
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		mousex = event.button.x;
		mousey = event.button.y;
		break;
	case SDL_MOUSEMOTION:
		mousex = event.motion.x;
		mousey = event.motion.y;
		break;
	default:
		break;
	}
}

void ResetFullscreen();

void RunGameState(sago::GameStateInterface& state ) {
	bool done = false;     //We are done!
	while (!done && !globalData.isShuttingDown) {
		if (globalData.resetVideo) {
			ResetFullscreen();
			globalData.resetVideo = false;
		}
		SDL_SetRenderDrawColor(globalData.screen, 0, 0, 0, 0);
		SDL_RenderClear(globalData.screen);
		state.Draw(globalData.screen);

		SDL_Delay(1);
		SDL_Event event;

		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				globalData.isShuttingDown = true;
				done = true;
			}

			if (event.type == SDL_WINDOWEVENT_RESIZED) {
				std::cout << event.window.data1 << ", " << event.window.data2 << "\n";
				SDL_GetRendererOutputSize(globalData.screen, &globalData.xsize, &globalData.ysize);
			}

			if (event.type == SDL_KEYDOWN) {
				if (!globalData.resetVideo && event.key.keysym.sym == SDLK_RETURN && event.key.keysym.mod & KMOD_LALT) {
					globalData.fullscreen = !globalData.fullscreen;
					globalData.resetVideo = true;
				}
			}

			bool processed = false;
			state.ProcessInput(event, processed);

		}

		state.Update();

		SDL_RenderPresent(globalData.screen);


		if (!state.IsActive()) {
			done = true;
		}
	}
}

void startWorld() {
	Game g;
	RunGameState(g);
}

static SDL_Window* win = nullptr;

void ResetFullscreen() {
	sago::SagoDataHolder& dataHolder = *globalData.dataHolder;
	Mix_HaltMusic();  //We need to reload all data in case the screen type changes. Music must be stopped before unload.
	if (globalData.fullscreen) {
		SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else {
		SDL_SetWindowFullscreen(win, 0);
	}
	dataHolder.invalidateAll(globalData.screen);
	globalData.spriteHolder.reset(new sago::SagoSpriteHolder( dataHolder ) );
	SDL_ShowCursor(SDL_ENABLE);
	SDL_GetRendererOutputSize(globalData.screen, &globalData.xsize, &globalData.ysize);
}

void toggleFullscreen() {
	globalData.fullscreen = !globalData.fullscreen;
	ResetFullscreen();
}

void runMenuOptions() {
	Menu m(globalData.screen, true);
	Button bFullscreen;
	bFullscreen.setLabel("Fullscreen");
	bFullscreen.setAction(toggleFullscreen);
	m.addButton(&bFullscreen);
	RunGameState(m);
}

void runGame() {
	int posX = 100, posY = 100, width = 1280, height = 800;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_OGG);

	globalData.fullscreen = Config::getInstance()->getInt("fullscreen");

	win = SDL_CreateWindow("Saland Adventures", posX, posY, width, height, 0);
	globalData.screen = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	//SDL_RenderSetLogicalSize(globalData.screen, 1024, 768);
	sago::SagoDataHolder holder(globalData.screen);
	globalData.spriteHolder.reset(new sago::SagoSpriteHolder(holder));
	globalData.dataHolder = &holder;
	ResetFullscreen();

	TitleScreen ts;
	RunGameState(ts);

	standardButton.setSurfaces();
	Menu m(globalData.screen, false);
	Button bStart;
	bStart.setLabel("Start");
	bStart.setAction(startWorld);
	m.addButton(&bStart);
	Button bOptions;
	bOptions.setLabel("Options");
	bOptions.setAction(runMenuOptions);
	m.addButton(&bOptions);
	if (!globalData.isShuttingDown) {
		RunGameState(m);
	}

	SDL_DestroyRenderer(globalData.screen);
	SDL_DestroyWindow(win);

	SDL_Quit();

}

int main(int argc, char* argv[]) {
	PHYSFS_init(argv[0]);
	PHYSFS_addToSearchPath((std::string(SHAREDIR)+"/packages/saland-0001.data").c_str(), 1);
	PHYSFS_addToSearchPath((std::string(PHYSFS_getBaseDir())+"/packages/saland-0001.data").c_str(), 1);
	PHYSFS_addToSearchPath((std::string(PHYSFS_getBaseDir())+"/data").c_str(), 1);
	std::string savepath = getPathToSaveFiles();
	OsCreateSaveFolder();
	PHYSFS_addToSearchPath(savepath.c_str(), 1);
	PHYSFS_setWriteDir(savepath.c_str());
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("fullscreen", "Run in fullscreen")
	("no-fullscreen", "Run in window")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version")) {
		std::cout << "saland " << VERSION_NUMBER << "\n";
		return 0;
	}
	if (vm.count("fullscreen")) {
		Config::getInstance()->setInt("fullscreen", 1);
	}
	if (vm.count("no-fullscreen")) {
		Config::getInstance()->setInt("fullscreen", 0);
	}
	Config::getInstance()->setDefault("fullscreen", "0");
	Config::getInstance()->setDefault("draw_collision", "0");
	runGame();
	Config::getInstance()->save();
	return 0;
}
