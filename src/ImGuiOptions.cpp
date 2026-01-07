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

#include "ImGuiOptions.hpp"
#include "Libs/imgui/imgui.h"
#include "saland/globals.hpp"
#include <SDL.h>

ImGuiOptions::ImGuiOptions() {
}

bool ImGuiOptions::IsActive() {
	return active;
}

void ImGuiOptions::Draw(SDL_Renderer* target) {
	// Center the menu window
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size(400, 250);
	ImVec2 window_pos(
	    (io.DisplaySize.x - window_size.x) * 0.5f,
	    (io.DisplaySize.y - window_size.y) * 0.5f
	);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	if (!ImGui::Begin("Options", &active,
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

	// Fullscreen toggle button
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("Toggle Fullscreen", ImVec2(button_width, button_height))) {
		pendingFullscreenToggle = true;
	}

	// Display current fullscreen status
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
	ImGui::SetCursorPosX(button_x);
	const char* status = globalData.fullscreen ? "Currently: Fullscreen" : "Currently: Windowed";
	ImGui::Text("%s", status);

	ImGui::End();
}

void ImGuiOptions::ProcessInput(const SDL_Event& event, bool& processed) {
	// Handle Escape key to close menu
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			active = false;
			processed = true;
		}
	}
}

void ImGuiOptions::Update() {
	if (pendingFullscreenToggle) {
		toggleFullscreen();
		pendingFullscreenToggle = false;
	}
}
