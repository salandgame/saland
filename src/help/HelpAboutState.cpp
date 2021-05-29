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

#include "DialogBox.hpp"
#include "HelpCommon.hpp"
#include "HelpAboutState.hpp"
#include "../global.hpp"
#include "../common.h"
#include "../MenuSystem.h"
#include "sstream"
#include "../version.h"
#include "../sago/SagoMisc.hpp"

const int buttonOffset = 160;



static void setHelpGamepadFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(30);
	field.SetOutline(1, {128,128,128,255});
	field.SetText(text);
}






HelpAboutState::HelpAboutState() {
	SDL_RendererInfo renderInfo;
	SDL_version compiled;
	SDL_version linked;
	SDL_GetRendererInfo(globalData.screen, &renderInfo);
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	const char* audio_driver_name = SDL_GetCurrentAudioDriver();
	if (!audio_driver_name) {
		audio_driver_name = _("No audio driver");
	}
	std::stringstream infoStream;
	infoStream << _("Name:") << " " << _("Saland Adventures") << "\n";
	infoStream << _("Original name:") << " Saland Adventures" << "\n";
	infoStream << _("Version:") << " " << VERSION_NUMBER << "\n";
	infoStream << _("Homepage:") << " " << "https://salandgame.github.io\n";
	infoStream << _("Github page:") << " " << "https://github.com/salandgame/saland\n";
	infoStream << _("SDL render:") << " " << renderInfo.name << "\n";
	infoStream << _("SDL audio driver:") << " " << audio_driver_name << "\n";
	infoStream << _("SDL compiled version:") << " " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << "\n";
	infoStream << _("SDL linked version:") << " " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << "\n";
	infoStream << _("Save folder:") << " " << PHYSFS_getWriteDir() << "\n";
	infoStream << _("Locale:") << " " << setlocale( LC_CTYPE, nullptr ) << "\n";
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), titleField, _("About"));
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), infoBox, infoStream.str().c_str());
	sago::WriteFileContent("about.txt", infoStream.str());
}

HelpAboutState::~HelpAboutState() {
}

bool HelpAboutState::IsActive() {
	return isActive;
}

void HelpAboutState::ProcessInput(const SDL_Event& event, bool& processed) {

	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isConfirmEvent(event) || isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

void HelpAboutState::Draw(SDL_Renderer* target) {
	//DrawBackground(target);
	titleField.Draw(target, 50, 50);
	DrawRectYellow(target, 40, 90, 600, 900);
	infoBox.SetMaxWidth(850);
	infoBox.Draw(target, 50, 100);
#if DEBUG
	static sago::SagoTextField mousePos;
	mousePos.SetHolder(&globalData.spriteHolder->GetDataHolder());
	mousePos.SetFontSize(16);
	mousePos.SetOutline(1, {128,128,128,255});
	mousePos.SetText(std::string("Mouse position: ")+std::to_string(globalData.mousex)+std::string(", ")+std::to_string(globalData.mousey));
	mousePos.Draw(target, 0,0);
#endif
}

void HelpAboutState::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		bMouseUp=true;
	}
}
