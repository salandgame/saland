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
#include "../global.hpp"
#include "../common.h"
#include "ReadKeyboard.h"
#include "utf8.h"
#include "../MenuSystem.h"

static void setButtonFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(24);
	field.SetOutline(1, {0,0,0,255});
	field.SetText(text);
}


bool OpenDialogbox(int x, int y, std::string& name, const std::string& header) {
	DialogBox d(x, y, name, header);
	RunGameState(d);
	if (d.IsUpdated()) {
		name = d.GetName();
		return true;
	}
	return false;
}

DialogBox::DialogBox(int x, int y, const std::string& name, const std::string& header) : header(header) {
	this->x = x;
	this->y = y;
	SetName(name);
	setButtonFont(&globalData.spriteHolder->GetDataHolder(), headerLabel, header.c_str());
	setButtonFont(&globalData.spriteHolder->GetDataHolder(), enterLabel, _("Enter to accept"));
	setButtonFont(&globalData.spriteHolder->GetDataHolder(), cancelLabel, _("Esc to cancel"));
	sagoTextSetBlueFont(textField);
	sagoTextSetBlueFont(cursorLabel);
	cursorLabel.SetText("|");
	for (auto position = virtualKeyboard.alphabet.begin(); position != virtualKeyboard.alphabet.end() ; utf8::advance(position, 1, virtualKeyboard.alphabet.end())) {
		auto endPosition = position;
		utf8::advance(endPosition, 1, virtualKeyboard.alphabet.end());
		std::string theChar(position, endPosition);
		virtualKeyboard.gamePadChars.push_back(theChar);
		virtualKeyboard.gamePadCharFields.emplace_back();
		sago::SagoTextField& tf = virtualKeyboard.gamePadCharFields.back();
		setButtonFont(&globalData.spriteHolder->GetDataHolder(), tf, theChar.c_str());
		if (globalData.verboseLevel) {
			std::cout << *position;
		}
	}
	if (globalData.verboseLevel) {
		std::cout << "\n";
	}
}


DialogBox::~DialogBox() {
}

bool DialogBox::IsActive() {
	return isActive;
}

static bool insideRect (int x, int y, int height, int width) {
	if (globalData.mousex < x) {
		return false;
	}
	if (globalData.mousex > x+width) {
		return false;
	}
	if (globalData.mousey < y) {
		return false;
	}
	if (globalData.mousey > y+height) {
		return false;
	}
	return true;
}

// For use with the mouse/gamepad keyboard
int keyboardRowLimit = 20;

void DialogBox::Draw(SDL_Renderer* target) {
	this->x = globalData.xsize/2-300;
	this->y = globalData.ysize/2-100;
	DrawRectYellow(target, x, y, 200, 600);
	headerLabel.Draw(target, x+300, y+20, sago::SagoTextField::Alignment::center);
	DrawRectYellow(target, x+25, y+128, 50, 250);
	enterLabel.Draw(target, x+150, y+140, sago::SagoTextField::Alignment::center);
	DrawRectYellow(target, x+325, y+128, 50, 250);
	cancelLabel.Draw(target, x+450, y+140, sago::SagoTextField::Alignment::center);
	DrawRectWhite(target, x+26, y+64, 54, 600-2*26);
	textField.SetText(rk->GetString());
	textField.Draw(target, x+40, y+76);
	std::string strHolder = rk->GetString();
	strHolder.erase((int)rk->CharsBeforeCursor());

	if (((SDL_GetTicks()/600)%2)==1) {
		int width = 0;
		textField.GetRenderedSize( strHolder.c_str(), &width);
		width -= 2;
		cursorLabel.Draw(target, x+40+width,y+76);
	}
	const sago::SagoSprite& marked = globalData.spriteHolder->GetSprite("i_level_check_box_marked");
	for (size_t i = 0; i<virtualKeyboard.gamePadCharFields.size(); ++i) {
		if (virtualKeyboard.selectedChar == static_cast<int>(i)) {
			marked.Draw(target, SDL_GetTicks(), globalData.xsize/2-400+(i%keyboardRowLimit)*40-5, globalData.ysize/2+150+(i/keyboardRowLimit)*40-5);
		}
		sago::SagoTextField& f = virtualKeyboard.gamePadCharFields.at(i);
		f.Draw(target, globalData.xsize/2-400+(i%keyboardRowLimit)*40, globalData.ysize/2+150+(i/keyboardRowLimit)*40);
	}
}


static bool isGamePadStartEvent(const SDL_Event& event) {
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START ) {
			return true;
		}
	}
	return false;
}

static bool isGamePadLEvent(const SDL_Event& event) {
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER ) {
			return true;
		}
	}
	return false;
}

static bool isGamePadREvent(const SDL_Event& event) {
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER ) {
			return true;
		}
	}
	return false;
}


void DialogBox::virtualKeyboardWriteSelectedChar(ReadKeyboard* rk, const std::string& insertChar) const {
	if (insertChar == virtualKeyboard.backspace) {
		rk->emulateBackspace();
	}
	else if (insertChar == virtualKeyboard.leftChar) {
		rk->cursorLeft();
	}
	else if (insertChar == virtualKeyboard.rightChar) {
		rk->cursorRight();
	}
	else {
		rk->putchar(insertChar);
	}
}


void DialogBox::ProcessInput(const SDL_Event& event, bool& processed) {
	if (event.type == SDL_TEXTINPUT) {
		if ((rk->ReadKey(event))&&(globalData.SoundEnabled)&&(!globalData.NoSound)) {
			//Mix_PlayChannel(1, globalData.typingChunk.get(), 0);
		}
	}

	if ( event.type == SDL_KEYDOWN ) {
		if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) ) {
			name = rk->GetString();
			updated = true;
			isActive = false;
		}
		else if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
			isActive = false;
		}
		else {
			if ((rk->ReadKey(event))&&(globalData.SoundEnabled)&&(!globalData.NoSound)) {
				//Mix_PlayChannel(1, globalData.typingChunk.get(), 0);
			}
		}
	}
	else {
		if (isGamePadStartEvent(event)) {
			name = rk->GetString();
			updated = true;
			isActive = false;
		}
		if (isEscapeEvent(event)) {
			isActive = false;
		}
		if (isConfirmEvent(event)) {
			const sago::SagoTextField& f = virtualKeyboard.gamePadCharFields.at(virtualKeyboard.selectedChar);
			const std::string& insertChar = f.GetText();
			virtualKeyboardWriteSelectedChar(rk.get(), insertChar);
		}
		if (event.type == SDL_CONTROLLERBUTTONDOWN) {
			if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X ) {
				rk->emulateBackspace();
			}
			if (event.cbutton.button == SDL_CONTROLLER_BUTTON_Y ) {
				rk->putchar(" ");
			}
		}
		if (isGamePadLEvent(event)) {
			rk->cursorLeft();
		}
		if (isGamePadREvent(event)) {
			rk->cursorRight();
		}
		if (isRightEvent(event)) {
			if ( (virtualKeyboard.selectedChar+1)/keyboardRowLimit != virtualKeyboard.selectedChar/keyboardRowLimit ) {
				virtualKeyboard.selectedChar -= keyboardRowLimit;
			}
			++virtualKeyboard.selectedChar;
			if (virtualKeyboard.selectedChar >= static_cast<int>(virtualKeyboard.gamePadCharFields.size())) {
				virtualKeyboard.selectedChar -= virtualKeyboard.selectedChar%keyboardRowLimit;
			}
		}
		if (isLeftEvent(event)) {
			if ( (virtualKeyboard.selectedChar-1)/keyboardRowLimit != virtualKeyboard.selectedChar/keyboardRowLimit || virtualKeyboard.selectedChar == 0 ) {
				virtualKeyboard.selectedChar += keyboardRowLimit;
			}
			--virtualKeyboard.selectedChar;
		}
		if (isDownEvent(event)) {
			virtualKeyboard.selectedChar+= keyboardRowLimit;
		}
		if (isUpEvent(event)) {
			virtualKeyboard.selectedChar -= keyboardRowLimit;
		}
		if (virtualKeyboard.selectedChar < 0) {
			virtualKeyboard.selectedChar = 0;
		}
		if (virtualKeyboard.selectedChar >= static_cast<int>(virtualKeyboard.gamePadCharFields.size())) {
			virtualKeyboard.selectedChar = virtualKeyboard.gamePadCharFields.size()-1;
		}
	}
	processed = true;
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		bMouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;
		if (insideRect(x+25, y+128, 50, 250)) {
			name = rk->GetString();
			updated = true;
			isActive = false;
		}
		if (insideRect(x+325, y+128, 50, 250)) {
			isActive = false;
		}
		for (size_t i = 0; i<virtualKeyboard.gamePadCharFields.size(); ++i) {
			sago::SagoTextField& f = virtualKeyboard.gamePadCharFields.at(i);
			auto topx = globalData.xsize/2-400+(i%keyboardRowLimit)*40-5;
			auto topy = globalData.ysize/2+150+(i/keyboardRowLimit)*40-5;
			if (insideRect(topx, topy, 30, 30)) {
				std::string insertChar = f.GetText();
				virtualKeyboardWriteSelectedChar(rk.get(), insertChar);
			}
		}
	}

	if (globalData.mousex != oldmousex || globalData.mousey != oldmousey) {
		for (size_t i = 0; i<virtualKeyboard.gamePadCharFields.size(); ++i) {
			auto topx = globalData.xsize/2-400+(i%keyboardRowLimit)*40-5;
			auto topy = globalData.ysize/2+150+(i/keyboardRowLimit)*40-5;
			if (insideRect(topx, topy, 30, 30)) {
				virtualKeyboard.selectedChar = i;
			}
		}
		oldmousex = globalData.mousex;
		oldmousey = globalData.mousey;
	}
}

void DialogBox::SetName(const std::string& name) {
	this->name = name;
	rk = std::make_shared<ReadKeyboard>(name.c_str());
}

std::string DialogBox::GetName() const {
	return name;
}

bool DialogBox::IsUpdated() const {
	return updated;
}
