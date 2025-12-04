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
#include "../globals.hpp"
#include <boost/tokenizer.hpp>
#include "../../Libs/imgui/imgui.h"



static std::map<std::string, ConsoleCommand*> commands;

void RegisterCommand(ConsoleCommand* command) {
	commands[command->getCommand()] = command;
}

struct HelpConsoleCommand : public ConsoleCommand {
	virtual std::string getCommand() const override {
		return "help";
	}

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

	virtual std::string helpMessage() const override {
		return "Displays this message";
	}
};

static HelpConsoleCommand hcc;

Console::Console() {
	RegisterCommand(&hcc);
	inputBuffer[0] = '\0';
	historyPos = -1;
	scrollToBottom = false;
}

Console::~Console() {
}

bool Console::IsActive() {
	return active;
};

void Console::Activate() {
	active = true;
}

void Console::Draw(SDL_Renderer* target) {
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(globalData.xsize) - 40, static_cast<float>(globalData.ysize) / 2), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Console", &active, ImGuiWindowFlags_NoCollapse)) {
		ImGui::End();
		return;
	}

	// Reserve space for input at the bottom
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar)) {
		// Display command history
		for (const auto& item : history) {
			ImVec4 color = item.isError ? ImVec4(1.0f, 0.25f, 0.25f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(item.text.c_str());
			ImGui::PopStyleColor();
		}

		// Auto-scroll to bottom when new content is added
		if (scrollToBottom) {
			ImGui::SetScrollHereY(1.0f);
			scrollToBottom = false;
		}
	}
	ImGui::EndChild();

	// Command input
	ImGui::Separator();
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;

	if (ImGui::InputText("Input", inputBuffer, IM_ARRAYSIZE(inputBuffer), input_flags, &TextEditCallbackStub, (void*)this)) {
		std::string command(inputBuffer);
		if (!command.empty()) {
			ProcessCommand(command);
			inputBuffer[0] = '\0';
			reclaim_focus = true;
		}
	}

	// Auto-focus on input field
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus) {
		ImGui::SetKeyboardFocusHere(-1);
	}

	ImGui::End();
}


static std::vector<std::string> splitByWhitespace(std::string const& line) {
	std::vector<std::string> arg;
	boost::escaped_list_separator<char> els('\\',' ','\"');
	boost::tokenizer<boost::escaped_list_separator<char> > tok(line, els);
	for (const std::string& item : tok) {
		arg.push_back(item);
	}
	return arg;
}

void Console::ProcessCommand(const std::string& command) {
	// Add command to history
	history.push_back({"> " + command, false});
	commandHistory.push_back(command);
	historyPos = -1;

	std::vector<std::string> commandVector = splitByWhitespace(command);

	if (commandVector.size() && commands.find(commandVector[0]) != commands.end()) {
		try {
			std::string ret = commands[commandVector[0]]->run(commandVector);
			history.push_back({ret, false});
		}
		catch (std::exception& e) {
			history.push_back({std::string(e.what()), true});
		}
	}
	else {
		history.push_back({"   \"" + command + "\" not recognized", false});
	}

	scrollToBottom = true;
}

void Console::ProcessInput(const SDL_Event& event, bool& processed) {
	// ImGui handles all input internally, we just need to handle console close
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE && event.key.keysym.mod & KMOD_LSHIFT) {
			active = false;
			processed = true;
		}
	}
}

int Console::TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
	Console* console = (Console*)data->UserData;
	return console->TextEditCallback(data);
}

int Console::TextEditCallback(ImGuiInputTextCallbackData* data) {
	switch (data->EventFlag) {
		case ImGuiInputTextFlags_CallbackHistory: {
			const int prev_history_pos = historyPos;
			if (data->EventKey == ImGuiKey_UpArrow) {
				if (historyPos == -1) {
					historyPos = commandHistory.size() - 1;
				}
				else if (historyPos > 0) {
					historyPos--;
				}
			}
			else if (data->EventKey == ImGuiKey_DownArrow) {
				if (historyPos != -1) {
					historyPos++;
					if (historyPos >= (int)commandHistory.size()) {
						historyPos = -1;
					}
				}
			}

			// Apply history
			if (prev_history_pos != historyPos) {
				const char* history_str = (historyPos >= 0) ? commandHistory[historyPos].c_str() : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
			break;
		}
	}
	return 0;
}

void Console::Update() {
	return;
}
