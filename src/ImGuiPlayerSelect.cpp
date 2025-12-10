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
#include <SDL.h>

ImGuiPlayerSelect::ImGuiPlayerSelect() {
	currentPlayer = Config::getInstance()->getString("player");
}

bool ImGuiPlayerSelect::IsActive() {
	return active;
}

void ImGuiPlayerSelect::Draw(SDL_Renderer* target) {
	// Center the menu window
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size(400, 300);
	ImVec2 window_pos(
		(io.DisplaySize.x - window_size.x) * 0.5f,
		(io.DisplaySize.y - window_size.y) * 0.5f
	);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	if (!ImGui::Begin("Player Select", &active,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse)) {
		ImGui::End();
		return;
	}

	// Center buttons horizontally
	float button_width = 300.0f;
	float button_height = 60.0f;
	float window_width = ImGui::GetWindowSize().x;
	float button_x = (window_width - button_width) * 0.5f;

	// Add some top spacing
	ImGui::SetCursorPosY(50);

	// Player 1 button
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("Player 1", ImVec2(button_width, button_height))) {
		pendingPlayer = "player1";
	}

	// Player 2 button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("Player 2", ImVec2(button_width, button_height))) {
		pendingPlayer = "player2";
	}

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

void ImGuiPlayerSelect::SelectPlayer(const std::string& playerName) {
	Config::getInstance()->setString("player", playerName);
	active = false;
}

void ImGuiPlayerSelect::Update() {
	if (!pendingPlayer.empty()) {
		SelectPlayer(pendingPlayer);
		pendingPlayer.clear();
	}
}
