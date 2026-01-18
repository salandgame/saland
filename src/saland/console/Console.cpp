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
#include "../GameItems.hpp"
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

static std::vector<std::string> splitByWhitespace(std::string const& line) {
	std::vector<std::string> arg;
	boost::escaped_list_separator<char> els('\\',' ','\"');
	boost::tokenizer<boost::escaped_list_separator<char> > tok(line, els);
	for (const std::string& item : tok) {
		arg.push_back(item);
	}
	return arg;
}

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

	// Reserve space for input at the bottom (including help text)
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() * 2.5f;
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
	ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackCompletion;

	if (ImGui::InputText("Input", inputBuffer, IM_ARRAYSIZE(inputBuffer), input_flags, &TextEditCallbackStub, (void*)this)) {
		std::string command(inputBuffer);
		if (!command.empty()) {
			pendingCommand = command;
			inputBuffer[0] = '\0';
			reclaim_focus = true;
		}
	}

	// Display help information
	std::string currentInput(inputBuffer);
	if (currentInput.length()) {
		// Extract the first word (the command) from the input
		std::vector<std::string> inputParts = splitByWhitespace(currentInput);
		std::string firstWord = inputParts.empty() ? currentInput : inputParts[0];
		// Check if it's a complete command
		if (commands.find(firstWord) != commands.end()) {
			// Show help message for the complete command
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Help: %s", commands[firstWord]->helpMessage().c_str());
		}
		else {
			// Show matching commands
			std::vector<std::string> matches;
			for (const auto& cmd : commands) {
				if (cmd.first.find(firstWord) == 0) {
					matches.push_back(cmd.first);
				}
			}
			if (matches.size()) {
				std::string matchText = "Matches: ";
				for (size_t i = 0; i < matches.size(); ++i) {
					if (i > 0) {
						matchText += ", ";
					}
					matchText += matches[i];
				}
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "%s", matchText.c_str());
			}
		}
	}

	// Auto-focus on input field
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus) {
		ImGui::SetKeyboardFocusHere(-1);
	}

	ImGui::End();
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
			data->BufDirty = true;
			data->CursorPos = data->BufTextLen;
			data->SelectionStart = data->SelectionEnd = data->CursorPos;
		}
		break;
	}
	case ImGuiInputTextFlags_CallbackCompletion: {
		// Bash-style tab completion (no cycling - user must type to disambiguate)
		std::string currentInput(data->Buf, data->BufTextLen);
		std::vector<std::string> parts = splitByWhitespace(currentInput);

		// Determine what we're completing
		bool completingCommand = parts.empty() || (parts.size() == 1 && !currentInput.empty() && currentInput.back() != ' ');
		bool completingItemArg = false;
		std::string wordToComplete;
		std::string prefix; // Text before the word being completed

		if (completingCommand) {
			// Completing the command itself
			wordToComplete = parts.empty() ? "" : parts[0];
			prefix = "";
		}
		else {
			// Completing an argument
			std::string command = parts[0];
			// Check if this command takes item names as arguments
			if ((command == "spawn_item" && parts.size() <= 2) ||
			    (command == "give" && parts.size() <= 2)) {
				completingItemArg = true;
				// Get the partial item name (or empty if just typed space)
				if (currentInput.back() == ' ') {
					wordToComplete = "";
					prefix = currentInput;
				}
				else if (parts.size() >= 2) {
					wordToComplete = parts[1];
					// Find where the last word starts
					size_t lastSpace = currentInput.rfind(' ');
					prefix = currentInput.substr(0, lastSpace + 1);
				}
				else {
					wordToComplete = "";
					prefix = currentInput + " ";
				}
			}
		}

		// Build completion candidates
		std::vector<std::string> candidates;
		if (completingCommand) {
			// Complete command names
			for (const auto& cmd : commands) {
				if (cmd.first.find(wordToComplete) == 0) {
					candidates.push_back(cmd.first);
				}
			}
		}
		else if (completingItemArg) {
			// Complete item names
			std::vector<std::string> itemNames = getItemNames();
			for (const auto& item : itemNames) {
				if (item.find(wordToComplete) == 0) {
					candidates.push_back(prefix + item);
				}
			}
		}

		if (candidates.empty()) {
			// No matches - do nothing
			break;
		}
		else if (candidates.size() == 1) {
			// Single match - complete it fully
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, candidates[0].c_str());
			data->BufDirty = true;
			data->CursorPos = data->BufTextLen;
			data->SelectionStart = data->SelectionEnd = data->CursorPos;
		}
		else {
			// Multiple matches - find and apply common prefix, then show options
			std::string commonPrefix = candidates[0];
			for (size_t i = 1; i < candidates.size(); ++i) {
				size_t j = 0;
				while (j < commonPrefix.size() && j < candidates[i].size() &&
				       commonPrefix[j] == candidates[i][j]) {
					++j;
				}
				commonPrefix = commonPrefix.substr(0, j);
			}

			// Apply common prefix if it's longer than current input
			if (commonPrefix.size() > currentInput.size()) {
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, commonPrefix.c_str());
				data->BufDirty = true;
				data->CursorPos = data->BufTextLen;
				data->SelectionStart = data->SelectionEnd = data->CursorPos;
			}

			// Show all matches in history
			std::string matchList = "Matches: ";
			for (size_t i = 0; i < candidates.size(); ++i) {
				if (i > 0) matchList += ", ";
				// Show just the completed word, not full command
				if (completingItemArg) {
					matchList += candidates[i].substr(prefix.size());
				}
				else {
					matchList += candidates[i];
				}
			}
			history.push_back({matchList, false});
			scrollToBottom = true;
		}
		break;
	}
	}

	return 0;
}

void Console::Update() {
	if (pendingCommand.length()) {
		ProcessCommand(pendingCommand);
		pendingCommand.clear();
	}
}
