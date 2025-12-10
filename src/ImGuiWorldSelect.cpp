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

#include "ImGuiWorldSelect.hpp"
#include "Libs/imgui/imgui.h"
#include "common.h"
#include <SDL.h>

ImGuiWorldSelect::ImGuiWorldSelect() {
	currentWorld = Config::getInstance()->getString("world");
}

bool ImGuiWorldSelect::IsActive() {
	return active;
}

void ImGuiWorldSelect::Draw(SDL_Renderer* target) {
	// Center the menu window
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size(400, 400);
	ImVec2 window_pos(
		(io.DisplaySize.x - window_size.x) * 0.5f,
		(io.DisplaySize.y - window_size.y) * 0.5f
	);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	if (!ImGui::Begin("World Select", &active,
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

	// World 1 button
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("World 1", ImVec2(button_width, button_height))) {
		pendingWorld = "world1";
	}

	// World 2 button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("World 2", ImVec2(button_width, button_height))) {
		pendingWorld = "world2";
	}

	// World 3 button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("World 3", ImVec2(button_width, button_height))) {
		pendingWorld = "world3";
	}

	// World 4 button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("World 4", ImVec2(button_width, button_height))) {
		pendingWorld = "world4";
	}

	ImGui::End();
}

void ImGuiWorldSelect::ProcessInput(const SDL_Event& event, bool& processed) {
	// Handle Escape key to close menu
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			active = false;
			processed = true;
		}
	}
}

void ImGuiWorldSelect::SelectWorld(const std::string& worldName) {
	Config::getInstance()->setString("world", worldName);
	active = false;
}

void ImGuiWorldSelect::Update() {
	if (!pendingWorld.empty()) {
		SelectWorld(pendingWorld);
		pendingWorld.clear();
	}
}
