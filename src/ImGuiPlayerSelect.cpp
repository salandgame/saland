/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2024 Poul Sander

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

#include "ImGuiPlayerSelect.hpp"
#include "Libs/imgui/imgui.h"
#include "common.h"
#include "saland/globals.hpp"
#include "sago/SagoMisc.hpp"
#include <SDL.h>
#include <algorithm>
#include <cstring>
#include <format>

ImGuiPlayerSelect::ImGuiPlayerSelect() {
	// Define available hair options
	hairOptions = {
		{"standard_hair", "Redhead"},
		{"hair_blonde", "Blonde"},
		{"hair_brunette", "Brunette"},
		{"hair_raven", "Black"},
		{"hair_blue", "Blue"},
		{"hair_1", "Short Blue"}
	};

	LoadPlayerList();
	LoadSelectedPlayer();
}

void ImGuiPlayerSelect::LoadPlayerList() {
	playerNames.clear();
	auto files = sago::GetFileList("players");
	for (const auto& file : files) {
		if (file.size() > 5 && file.substr(file.size() - 5) == ".json") {
			std::string playerName = file.substr(0, file.size() - 5);
			playerNames.push_back(playerName);
		}
	}
	std::sort(playerNames.begin(), playerNames.end());

	// Find current player in list
	std::string currentPlayer = Config::getInstance()->getString("player");
	selectedPlayerIndex = 0;
	for (size_t i = 0; i < playerNames.size(); i++) {
		if (playerNames[i] == currentPlayer) {
			selectedPlayerIndex = i;
			break;
		}
	}
}

void ImGuiPlayerSelect::LoadSelectedPlayer() {
	if (selectedPlayerIndex >= 0 && selectedPlayerIndex < static_cast<int>(playerNames.size())) {
		std::string filename = std::format("players/{}.json", playerNames[selectedPlayerIndex]);
		if (sago::FileExists(filename.c_str())) {
			nlohmann::json j = nlohmann::json::parse(sago::GetFileContent(filename.c_str()));
			editingPlayer = j;
			editingPlayer.save_name = playerNames[selectedPlayerIndex];
		} else {
			editingPlayer = Player();
			editingPlayer.save_name = playerNames[selectedPlayerIndex];
		}
	} else {
		editingPlayer = Player();
		editingPlayer.save_name = "player1";
	}

	// Update UI buffers
	strncpy(nameBuffer, editingPlayer.save_name.c_str(), sizeof(nameBuffer) - 1);
	nameBuffer[sizeof(nameBuffer) - 1] = '\0';

	// Set race index
	selectedRaceIndex = (editingPlayer.race == "male") ? 0 : 1;

	// Set hair index
	selectedHairIndex = 0;
	for (size_t i = 0; i < hairOptions.size(); i++) {
		if (hairOptions[i].id == editingPlayer.hair) {
			selectedHairIndex = i;
			break;
		}
	}

	needsSave = false;
}

void ImGuiPlayerSelect::SaveCurrentPlayer() {
	editingPlayer.save_name = std::string(nameBuffer);
	nlohmann::json j = editingPlayer;
	sago::WriteFileContent(std::format("players/{}.json", editingPlayer.save_name).c_str(), j.dump());
	needsSave = false;
}

void ImGuiPlayerSelect::CreateNewPlayer() {
	int newPlayerNum = 1;
	std::string newName;
	do {
		newName = std::format("player{}", newPlayerNum);
		newPlayerNum++;
	} while (std::find(playerNames.begin(), playerNames.end(), newName) != playerNames.end());

	playerNames.push_back(newName);
	std::sort(playerNames.begin(), playerNames.end());

	// Find the new player index
	for (size_t i = 0; i < playerNames.size(); i++) {
		if (playerNames[i] == newName) {
			selectedPlayerIndex = i;
			break;
		}
	}

	editingPlayer = Player();
	editingPlayer.save_name = newName;
	strncpy(nameBuffer, newName.c_str(), sizeof(nameBuffer) - 1);
	nameBuffer[sizeof(nameBuffer) - 1] = '\0';

	SaveCurrentPlayer();
}

void ImGuiPlayerSelect::DeleteCurrentPlayer() {
	if (playerNames.size() <= 1) {
		return; // Don't delete the last player
	}

	std::string toDelete = playerNames[selectedPlayerIndex];
	playerNames.erase(playerNames.begin() + selectedPlayerIndex);

	if (selectedPlayerIndex >= static_cast<int>(playerNames.size())) {
		selectedPlayerIndex = playerNames.size() - 1;
	}

	needsRefresh = true;
}

bool ImGuiPlayerSelect::IsActive() {
	return active;
}

void ImGuiPlayerSelect::Draw(SDL_Renderer* target) {
	// Center the menu window
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size(600, 500);
	ImVec2 window_pos(
		(io.DisplaySize.x - window_size.x) * 0.5f,
		(io.DisplaySize.y - window_size.y) * 0.5f
	);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	if (!ImGui::Begin("Player Management", &active,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse)) {
		ImGui::End();
		return;
	}

	// Left panel - Player list
	ImGui::BeginChild("PlayerList", ImVec2(200, 0), true);
	ImGui::Text("Players");
	ImGui::Separator();

	for (size_t i = 0; i < playerNames.size(); i++) {
		if (ImGui::Selectable(playerNames[i].c_str(), selectedPlayerIndex == static_cast<int>(i))) {
			if (needsSave) {
				SaveCurrentPlayer();
			}
			selectedPlayerIndex = i;
			LoadSelectedPlayer();
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if (ImGui::Button("New Player", ImVec2(-1, 30))) {
		if (needsSave) {
			SaveCurrentPlayer();
		}
		CreateNewPlayer();
	}

	if (playerNames.size() > 1) {
		if (ImGui::Button("Delete", ImVec2(-1, 30))) {
			DeleteCurrentPlayer();
		}
	}

	ImGui::EndChild();

	ImGui::SameLine();

	// Right panel - Player details
	ImGui::BeginChild("PlayerDetails", ImVec2(0, 0), true);
	ImGui::Text("Player Details");
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	// Name input
	ImGui::Text("Name:");
	ImGui::PushItemWidth(-1);
	if (ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer))) {
		needsSave = true;
	}
	ImGui::PopItemWidth();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	// Gender/Race selection
	ImGui::Text("Gender:");
	const char* races[] = { "Male", "Female" };
	if (ImGui::Combo("##race", &selectedRaceIndex, races, 2)) {
		editingPlayer.race = (selectedRaceIndex == 0) ? "male" : "female";
		needsSave = true;
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	// Hair selection
	ImGui::Text("Hair Style:");
	std::vector<const char*> hairNames;
	for (const auto& option : hairOptions) {
		hairNames.push_back(option.displayName.c_str());
	}
	if (ImGui::Combo("##hair", &selectedHairIndex, hairNames.data(), hairNames.size())) {
		editingPlayer.hair = hairOptions[selectedHairIndex].id;
		needsSave = true;
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	// Action buttons
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	float button_width = (ImGui::GetContentRegionAvail().x - 10) / 2;

	if (ImGui::Button("Save & Select", ImVec2(button_width, 40))) {
		SaveCurrentPlayer();
		Config::getInstance()->setString("player", editingPlayer.save_name);
		globalData.player = editingPlayer;
		active = false;
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel", ImVec2(button_width, 40))) {
		active = false;
	}

	ImGui::EndChild();

	ImGui::End();
}

void ImGuiPlayerSelect::ProcessInput(const SDL_Event& event, bool& processed) {
	// Handle Escape key to close menu
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			active = false;
			processed = true;
		}
	}
}

void ImGuiPlayerSelect::Update() {
	if (needsRefresh) {
		LoadPlayerList();
		LoadSelectedPlayer();
		needsRefresh = false;
	}
}
