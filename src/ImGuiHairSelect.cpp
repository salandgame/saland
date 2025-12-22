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

#include "ImGuiHairSelect.hpp"
#include "Libs/imgui/imgui.h"
#include "common.h"
#include "saland/globals.hpp"
#include <SDL.h>

ImGuiHairSelect::ImGuiHairSelect() {
	currentHair = globalData.player.hair;
	
	// Define available hair options
	hairOptions = {
		{"standard_hair", "Redhead"},
		{"hair_blonde", "Blonde"},
		{"hair_brunette", "Brunette"},
		{"hair_raven", "Black"},
		{"hair_blue", "Blue"},
		{"hair_1", "Short Blue"}  // The old male_hair_single_blue variant
	};
}

bool ImGuiHairSelect::IsActive() {
	return active;
}

void ImGuiHairSelect::Draw(SDL_Renderer* target) {
	// Center the menu window
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size(400, 500);
	ImVec2 window_pos(
		(io.DisplaySize.x - window_size.x) * 0.5f,
		(io.DisplaySize.y - window_size.y) * 0.5f
	);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	if (!ImGui::Begin("Hair Select", &active,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse)) {
		ImGui::End();
		return;
	}

	// Center buttons horizontally
	float button_width = 300.0f;
	float button_height = 50.0f;
	float window_width = ImGui::GetWindowSize().x;
	float button_x = (window_width - button_width) * 0.5f;

	// Add some top spacing
	ImGui::Dummy(ImVec2(0.0f, 30.0f));

	// Display hair options as buttons
	for (const auto& option : hairOptions) {
		ImGui::SetCursorPosX(button_x);
		
		// Highlight current selection
		if (option.id == currentHair) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
		}
		
		if (ImGui::Button(option.displayName.c_str(), ImVec2(button_width, button_height))) {
			pendingHair = option.id;
		}
		
		if (option.id == currentHair) {
			ImGui::PopStyleColor(3);
		}
		
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
	}

	ImGui::End();
}

void ImGuiHairSelect::ProcessInput(const SDL_Event& event, bool& processed) {
	// Handle Escape key to close menu
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			active = false;
			processed = true;
		}
	}
}

void ImGuiHairSelect::SelectHair(const std::string& hairName) {
	globalData.player.hair = hairName;
	currentHair = hairName;
	// Save player data
	PlayerSave();
	active = false;
}

void ImGuiHairSelect::Update() {
	if (!pendingHair.empty()) {
		SelectHair(pendingHair);
		pendingHair.clear();
	}
}
