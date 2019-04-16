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
#include <boost/tokenizer.hpp>

static void SetFieldValues(sago::SagoTextField& field) {
	field.SetHolder(&globalData.spriteHolder->GetDataHolder());
	field.SetFont("freeserif");
	field.SetFontSize(20);
	field.SetOutline(1, {64,64,64,255});
}

static std::map<std::string, ConsoleCommand*> commands;

void RegisterCommand(ConsoleCommand* command) {
	commands[command->getCommand()] = command;
}

struct HelpConsoleCommand : public ConsoleCommand {
	virtual std::string getCommand() const override {return "help"; }

	virtual std::string run(const std::vector<std::string>& args) override {
		if (args.size() > 1 && commands.find(args.at(1)) != commands.end() ) {
			std::stringstream ss;
			ss << "Command \"" << args.at(1) << "\": " << commands[args.at(1)]->helpMessage();
			return ss.str();
		}
		std::string helpMessage = "Allowed commands: ";
		for (const auto& cmd : commands) {
			helpMessage += cmd.first + ", ";
		}
		return helpMessage;
	}

	virtual std::string helpMessage() const override { return "Displays this message"; }
};

static HelpConsoleCommand hcc;

Console::Console() {
	RegisterCommand(&hcc);
	editPosition = editLine.begin();
	SDL_StartTextInput();
	editField.SetHolder(&globalData.spriteHolder->GetDataHolder());
	editField.SetFont("freeserif");
	editField.SetFontSize(20);
	editField.SetOutline(1, {64,64,64,255});
	editFieldMarker.SetHolder(&globalData.spriteHolder->GetDataHolder());
	editFieldMarker.SetFont("freeserif");
	editFieldMarker.SetFontSize(20);
	editFieldMarker.SetOutline(1, {64,64,64,255});
	editFieldMarker.SetText(">");
}

Console::~Console() {
	SDL_StopTextInput();
}

bool Console::IsActive() {
	return active;
};

void Console::Activate() {
	active = true;
}

void Console::Draw(SDL_Renderer* target) {
	int sideBoarder = 20;
	DrawRectYellow(target, sideBoarder, 10, globalData.ysize/2, globalData.xsize - sideBoarder*2);
	editField.SetText(editLine);
	editFieldMarker.Draw(target, sideBoarder+10, globalData.ysize/2-16);
	editField.Draw(target, sideBoarder+24, globalData.ysize/2-16);
	int j = 1;
	for (int i = (int)historyField.size()-1; i > -1; --i, ++j) {
		historyField.at(i).Draw(target, sideBoarder+10, globalData.ysize/2-16-j*22);
	}
}

void Console::putchar(const std::string& thing) {
	if (thing == "" || thing == "\n") {
		return;
	}
	int oldPostition = utf8::distance(editLine.begin(), editPosition);
	int lengthOfInsertString = utf8::distance(thing.begin(), thing.end());
	editLine.insert(editPosition, thing.begin(), thing.end());
	editPosition = editLine.begin();  //Inserting may destroy our old iterator
	utf8::advance(editPosition, oldPostition + lengthOfInsertString, editLine.end());
}

void Console::removeChar() {
	if (editPosition < editLine.end()) {
		std::string::iterator endChar= editPosition;
		utf8::advance(endChar, 1, editLine.end());
		editLine.erase(editPosition, endChar);
	}
}

static std::vector<std::string> splitByWhitespace(std::string const &line) { 
	std::vector<std::string> arg;
	boost::escaped_list_separator<char> els('\\',' ','\"');
	boost::tokenizer<boost::escaped_list_separator<char> > tok(line, els);
	for (const std::string& item : tok) {
		arg.push_back(item);
	}
	return arg;
}

void Console::ProcessCommand(const std::string& command) {
	sago::SagoTextField f;
	SetFieldValues(f);
	f.SetText(command);
	historyField.push_back(std::move(f));
	std::vector<std::string> commandVector = splitByWhitespace(command);
	sago::SagoTextField response;
	SetFieldValues(response);
	if (commandVector.size() && commands.find(commandVector[0]) != commands.end() ) {
		try {
			std::string ret = commands[commandVector[0]]->run(commandVector);
			response.SetText(ret);
		} catch (std::exception& e) {
			response.SetText(e.what());
		}
	}
	else {
		response.SetText(std::string("   \"")+command+"\" not recognized");
	}
	historyField.push_back(std::move(response));
}

bool Console::ReadKey(SDL_Keycode keyPressed) {
	if (keyPressed == SDLK_DELETE) {
		if ((editLine.length()>0)&& (editPosition<editLine.end())) {
			removeChar();
		}
		return true;
	}
	if (keyPressed == SDLK_BACKSPACE) {
		if (editPosition>editLine.begin()) {
			utf8::prior(editPosition, editLine.begin());
			removeChar();
			return true;
		}
		return false;
	}
	if (keyPressed == SDLK_HOME) {
		editPosition = editLine.begin();
		return true;
	}
	if (keyPressed == SDLK_END) {
		editPosition=editLine.end();
		return true;
	}
	if ((keyPressed == SDLK_LEFT) && (editPosition>editLine.begin())) {
		utf8::prior(editPosition, editLine.begin());
		return true;
	}
	if ((keyPressed == SDLK_RIGHT) && (editPosition<editLine.end())) {
		utf8::next(editPosition, editLine.end());
		return true;
	}
	if (keyPressed == SDLK_RETURN || keyPressed == SDLK_RETURN2) {
		if (editLine.length() > 0) {
			commandHistory.push_back(editLine);
			ProcessCommand(editLine);
		}
		editLine.clear();
		editPosition = editLine.begin();
		return true;
	}
	return false;
}

void Console::ProcessInput(const SDL_Event& event, bool &processed) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE && event.key.keysym.mod & KMOD_LSHIFT) {
			active = false;
			processed = true;
		}
		else {
			processed = ReadKey(event.key.keysym.sym);
		}
	}
	if (event.type == SDL_TEXTINPUT) {
		putchar(event.text.text);
	}
}

void Console::Update() {
	return;
}
