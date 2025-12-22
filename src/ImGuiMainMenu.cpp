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

#include "ImGuiMainMenu.hpp"
#include "Libs/imgui/imgui.h"
#include "common.h"
#include "saland/globals.hpp"
#include <SDL.h>
#include <format>

ImGuiMainMenu::ImGuiMainMenu() {
	UpdateLabels();
}

bool ImGuiMainMenu::IsActive() {
	return active;
}

void ImGuiMainMenu::UpdateLabels() {
	worldLabel = std::format("World: {}", Config::getInstance()->getString("world"));
	playerLabel = std::format("Player: {}", Config::getInstance()->getString("player"));
}

void ImGuiMainMenu::Draw(SDL_Renderer* target) {
	// Center the menu window
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size(400, 500);
	ImVec2 window_pos(
		(io.DisplaySize.x - window_size.x) * 0.5f,
		(io.DisplaySize.y - window_size.y) * 0.5f
	);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	if (!ImGui::Begin("Saland Adventures", &active,
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
	ImGui::Dummy(ImVec2(0.0f, 30.0f));

	// Start button
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("Start", ImVec2(button_width, button_height))) {
		pendingAction = PendingAction::StartGame;
	}

	// World Select button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button(worldLabel.c_str(), ImVec2(button_width, button_height))) {
		pendingAction = PendingAction::WorldSelect;
	}

	// Player Select button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button(playerLabel.c_str(), ImVec2(button_width, button_height))) {
		pendingAction = PendingAction::PlayerSelect;
	}

	// Options button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("Options", ImVec2(button_width, button_height))) {
		pendingAction = PendingAction::Options;
	}

	// About button
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::SetCursorPosX(button_x);
	if (ImGui::Button("About", ImVec2(button_width, button_height))) {
		pendingAction = PendingAction::About;
	}

	ImGui::End();
}

void ImGuiMainMenu::ProcessInput(const SDL_Event& event, bool& processed) {
	// Handle Escape key to close menu
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			active = false;
			processed = true;
		}
	}
}

void ImGuiMainMenu::ExecutePendingAction() {
	switch (pendingAction) {
		case PendingAction::StartGame:
			runStartGame();
			active = false;
			break;
		case PendingAction::WorldSelect:
			runWorldSelect();
			UpdateLabels();
			break;
		case PendingAction::PlayerSelect:
			runPlayerSelect();
			UpdateLabels();
			break;
		case PendingAction::Options:
			runMenuOptions();
			break;
		case PendingAction::About:
			runHelpAbout();
			break;
		case PendingAction::None:
			// No action to execute
			break;
	}
	pendingAction = PendingAction::None;
}

void ImGuiMainMenu::Update() {
	if (pendingAction != PendingAction::None) {
		ExecutePendingAction();
	}
}
