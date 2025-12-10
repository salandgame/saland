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
https://github.com/salandgame/saland
===========================================================================
*/

#include <iostream>
#include <boost/program_options.hpp>
#include <SDL_mixer.h>
#include "editor/SagoTextureSelector.hpp"
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

#include "SagoImGui.hpp"

#include "sago/SagoMisc.hpp"
#include "platform_folders.h"
#include "sagotmx/tmx_struct.h"
#include "sago/SagoTextBox.hpp"

#include "MenuSystem.h"
#include "help/HelpAboutState.hpp"
#include "ImGuiMainMenu.hpp"
#include "ImGuiWorldSelect.hpp"
#include "ImGuiPlayerSelect.hpp"
#include "ImGuiOptions.hpp"

#include "common.h"
#include "os.hpp"
#include "version.h"
#include <format>

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


std::string pathToScreenShots() {
	Config::getInstance()->setDefault("screenshot_dir", "screenshots");
	std::string screenshot_dir = Config::getInstance()->getString("screenshot_dir");
	if (OsPathIsRelative(screenshot_dir)) {
		return getPathToSaveFiles() + "/" + screenshot_dir;
	}
	return screenshot_dir;
}

static SDL_Window* win = nullptr;

//writeScreenShot saves the screen as a bmp file, it uses the time to get a unique filename
void writeScreenShot() {
	if (globalData.verboseLevel) {
		std::cout << "Saving screenshot" << "\n";
	}
	int rightNow = (int)time(nullptr);
	SDL_Surface* infoSurface = SDL_GetWindowSurface(win);
	if (!infoSurface) {
		std::cerr << "Could not get infoSurface. No screenshot written. Be aware that the screenshot feature only works with software render\n";
		return;
	}
	std::vector<char> pixels(infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel);
	int errorCode = SDL_RenderReadPixels(globalData.screen, &infoSurface->clip_rect, infoSurface->format->format, static_cast<void*>(pixels.data()), infoSurface->w * infoSurface->format->BytesPerPixel);
	if (errorCode) {
		SDL_FreeSurface(infoSurface);
		std::cerr << "Could not do SDL_RenderReadPixels. Error code: " << errorCode << ". No screenshot written\n";
		return;
	}
	SDL_Surface* sreenshotSurface = SDL_CreateRGBSurfaceFrom(static_cast<void*>(pixels.data()), infoSurface->w, infoSurface->h, infoSurface->format->BitsPerPixel, infoSurface->w * infoSurface->format->BytesPerPixel, infoSurface->format->Rmask, infoSurface->format->Gmask, infoSurface->format->Bmask, infoSurface->format->Amask);
	SDL_FreeSurface(infoSurface);
	if (!sreenshotSurface) {
		std::cerr << "Could not get sreenshotSurface. No screenshot written\n";
		return;
	}
	OsCreateFolder(pathToScreenShots());
	std::string buf = pathToScreenShots() + "/screenshot"+std::to_string(rightNow)+".bmp";
	SDL_SaveBMP(sreenshotSurface, buf.c_str());
	SDL_FreeSurface(sreenshotSurface);
	if (!globalData.NoSound) {
		if (globalData.SoundEnabled) {
			Mix_PlayChannel(1, globalData.dataHolder->getSoundHandler("cameraclick").get(), 0);
		}
	}
}


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
		int w=1;
		int h=1;
		SDL_GetRendererOutputSize(globalData.screen, &w, &h);
		globalData.logicalResize.SetPhysicalSize(w, h);
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		state.Draw(globalData.screen);
		ImGui::Render();
		ImGui_ImplSDLRenderer2_RenderDrawData( ImGui::GetDrawData(), globalData.screen );

		//While using Dear ImGui we do not draw the mouse ourself. This is gone: globalData.mouse.Draw(globalData.screen, SDL_GetTicks(), globalData.mousex, globalData.mousey);
		SDL_RenderPresent(globalData.screen);

		SDL_Delay(1);
		SDL_Event event;
		bool mustWriteScreenshot = false;

		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				globalData.isShuttingDown = true;
				done = true;
			}

			if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					std::cout << event.window.data1 << ", " << event.window.data2 << "\n";
					SDL_GetRendererOutputSize(globalData.screen, &globalData.xsize, &globalData.ysize);
				}
			}

			if (event.type == SDL_KEYDOWN) {
				if (!globalData.resetVideo && event.key.keysym.sym == SDLK_RETURN && event.key.keysym.mod & KMOD_LALT) {
					globalData.fullscreen = !globalData.fullscreen;
					globalData.resetVideo = true;
				}
				if ( event.key.keysym.sym == SDLK_F9 ) {
					mustWriteScreenshot = true;
				}
			}

			if (mustWriteScreenshot) {
				writeScreenShot();
			}
			bool processed = false;
			ImGui_ImplSDL2_ProcessEvent(&event);
			state.ProcessInput(event, processed);

		}


		state.Update();

		//SDL_RenderPresent(globalData.screen);


		if (!state.IsActive()) {
			done = true;
		}
	}
}


void runStartGame() {
	std::string playername = Config::getInstance()->getString("player");
	if (playername == "player2") {
		globalData.player.race = "male";
	}
	Game g;
	RunGameState(g);
}

void runEditor() {
	SagoTextureSelector sts;
	sts.Init();
	RunGameState(sts);
}

class ButtonConfigValue : public Button {
private:
	std::string config_key;
	std::string config_value;
public:
	ButtonConfigValue() {
		setPopOnRun(true);
	}

	void setConfigValues(const std::string& key, const std::string& value) {
		config_key = key;
		config_value = value;
	}

	virtual void doAction() override {
		Config::getInstance()->setString(config_key, config_value);
	}
};

void runWorldSelect() {
	ImGuiWorldSelect m;
	RunGameState(m);
}

void runPlayerSelect() {
	ImGuiPlayerSelect m;
	RunGameState(m);
}


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
	//SDL_GetRendererOutputSize(globalData.screen, &globalData.xsize, &globalData.ysize);
}

void toggleFullscreen() {
	globalData.fullscreen = !globalData.fullscreen;
	ResetFullscreen();
}

void runMenuOptions() {
	ImGuiOptions m;
	RunGameState(m);
}

void runHelpAbout() {
	HelpAboutState helpAbout;
	RunGameState(helpAbout);
}

class WorldSelectButton : public Button {
private:
	void UpdateLabel() {
		this->setLabel(std::format("World: {}", Config::getInstance()->getString("world") ));
	}
public:
	WorldSelectButton() {
		UpdateLabel();
	}

	virtual void doAction() override {
		runWorldSelect();
		UpdateLabel();
	}
};

class PlayerSelectButton : public Button {
private:
	void UpdateLabel() {
		this->setLabel(std::format("Player: {}", Config::getInstance()->getString("player") ));
	}
public:
	PlayerSelectButton() {
		UpdateLabel();
	}

	virtual void doAction() override {
		runPlayerSelect();
		UpdateLabel();
	}
};


void runGame() {
	globalData.xsize = 1280;
	globalData.ysize = 800;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_OGG);
	if (!globalData.NoSound) {
		//If sound has not been disabled, then load the sound system
		if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
			std::cerr << "Warning: Couldn't set 44100 Hz 16-bit audio - Reason: " << SDL_GetError() << "\n"
			          << "Sound will be disabled!" << "\n";
			globalData.NoSound = true; //Tries to stop all sound from playing/loading
		}
	}

	int rendererFlags = 0;

	globalData.fullscreen = Config::getInstance()->getInt("fullscreen");
	if (Config::getInstance()->getInt("always-software")) {
		rendererFlags |= SDL_RENDERER_SOFTWARE;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SCALING, "1");
	win = SDL_CreateWindow("Saland Adventures", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, globalData.xsize, globalData.ysize, SDL_WINDOW_RESIZABLE);
	globalData.screen = SDL_CreateRenderer(win, -1, rendererFlags);

	//SDL_RenderSetLogicalSize(globalData.screen, globalData.xsize, globalData.ysize);
	InitImGui(win, globalData.screen, globalData.xsize, globalData.ysize);
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	std::string imgui_inifile = getPathToSaveFiles() + "/imgui.ini";
	ImGui::LoadIniSettingsFromDisk(imgui_inifile.c_str());

	globalData.logicalResize = sago::SagoLogicalResize(1280, 720);

	sago::SagoDataHolder holder(globalData.screen);
	globalData.spriteHolder.reset(new sago::SagoSpriteHolder(holder));
	globalData.dataHolder = &holder;
	ResetFullscreen();

	if (globalData.editor) {
		runEditor();
		globalData.isShuttingDown = true;
	}

	TitleScreen ts;
	RunGameState(ts);

	standardButton.setSurfaces();
	ImGuiMainMenu m;
	if (!globalData.isShuttingDown) {
		RunGameState(m);
	}

	ImGui::SaveIniSettingsToDisk(imgui_inifile.c_str());

	SDL_DestroyRenderer(globalData.screen);
	SDL_DestroyWindow(win);

	SDL_Quit();

}

int main(int argc, char* argv[]) {
	PHYSFS_init(argv[0]);
	PHYSFS_mount((std::string(SHAREDIR)+"/packages/saland-0001.data").c_str(), nullptr, 0);
	PHYSFS_mount((std::string(PHYSFS_getBaseDir())+"/packages/saland-0001.data").c_str(), nullptr, 0);
	PHYSFS_mount((std::string(PHYSFS_getBaseDir())+"/data").c_str(), nullptr, 0);
	std::string savepath = getPathToSaveFiles();
	OsCreateSaveFolder();
	PHYSFS_mount(savepath.c_str(), nullptr, 0);
	PHYSFS_setWriteDir(savepath.c_str());
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("editor", "Start the editor")
	("fullscreen", "Run in fullscreen")
	("no-fullscreen", "Run in window")
	("software-renderer", boost::program_options::value<int>(), "Asks SDL2 to use software renderer")
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
	if (vm.count("editor")) {
		globalData.editor = true;
	}
	if (vm.count("fullscreen")) {
		Config::getInstance()->setInt("fullscreen", 1);
	}
	if (vm.count("no-fullscreen")) {
		Config::getInstance()->setInt("fullscreen", 0);
	}
	if (vm.count("software-renderer")) {
		Config::getInstance()->setInt("always-software", vm["software-renderer"].as<int>());
	}
	Config::getInstance()->setDefault("fullscreen", "0");
	Config::getInstance()->setDefault("draw_collision", "0");
	Config::getInstance()->setDefault("world", "world1");
	Config::getInstance()->setDefault("player", "player1");
	runGame();
	Config::getInstance()->save();
	return 0;
}
