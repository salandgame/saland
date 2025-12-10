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

#ifndef IMGUI_MAIN_MENU_HPP
#define IMGUI_MAIN_MENU_HPP

#include "sago/GameStateInterface.hpp"
#include <string>

struct SDL_Renderer;
union SDL_Event;

// Forward declarations for menu actions
void runStartGame();
void runWorldSelect();
void runPlayerSelect();
void runMenuOptions();
void runHelpAbout();

class ImGuiMainMenu : public sago::GameStateInterface {
public:
	ImGuiMainMenu();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool& processed) override;
	virtual void Update() override;

	ImGuiMainMenu(const ImGuiMainMenu&) = delete;
	ImGuiMainMenu& operator=(const ImGuiMainMenu&) = delete;
	virtual ~ImGuiMainMenu() = default;

private:
	enum class PendingAction {
		None,
		StartGame,
		WorldSelect,
		PlayerSelect,
		Options,
		About
	};

	void UpdateLabels();
	void ExecutePendingAction();

	bool active = true;
	std::string worldLabel;
	std::string playerLabel;
	PendingAction pendingAction = PendingAction::None;
};

#endif /* IMGUI_MAIN_MENU_HPP */
