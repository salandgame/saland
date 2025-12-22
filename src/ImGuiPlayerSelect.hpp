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

#ifndef IMGUI_PLAYER_SELECT_HPP
#define IMGUI_PLAYER_SELECT_HPP

#include "sago/GameStateInterface.hpp"
#include "saland/model/Player.hpp"
#include <string>
#include <vector>

struct SDL_Renderer;
union SDL_Event;

class ImGuiPlayerSelect : public sago::GameStateInterface {
public:
	ImGuiPlayerSelect();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool& processed) override;
	virtual void Update() override;

	ImGuiPlayerSelect(const ImGuiPlayerSelect&) = delete;
	ImGuiPlayerSelect& operator=(const ImGuiPlayerSelect&) = delete;
	virtual ~ImGuiPlayerSelect() = default;

private:
	void LoadPlayerList();
	void LoadSelectedPlayer();
	void SaveCurrentPlayer();
	void CreateNewPlayer();
	void DeleteCurrentPlayer();

	bool active = true;
	std::vector<std::string> playerNames;
	int selectedPlayerIndex = 0;
	Player editingPlayer;
	char nameBuffer[64] = "";
	int selectedRaceIndex = 0;
	int selectedHairIndex = 0;

	struct HairOption {
		std::string id;
		std::string displayName;
	};
	std::vector<HairOption> hairOptions;

	bool needsSave = false;
	bool needsRefresh = false;
};

#endif /* IMGUI_PLAYER_SELECT_HPP */
