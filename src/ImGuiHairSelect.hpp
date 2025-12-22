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

#ifndef IMGUI_HAIR_SELECT_HPP
#define IMGUI_HAIR_SELECT_HPP

#include "sago/GameStateInterface.hpp"
#include <string>
#include <vector>

struct SDL_Renderer;
union SDL_Event;

class ImGuiHairSelect : public sago::GameStateInterface {
public:
	ImGuiHairSelect();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool& processed) override;
	virtual void Update() override;

	ImGuiHairSelect(const ImGuiHairSelect&) = delete;
	ImGuiHairSelect& operator=(const ImGuiHairSelect&) = delete;
	virtual ~ImGuiHairSelect() = default;

private:
	void SelectHair(const std::string& hairName);

	bool active = true;
	std::string pendingHair;
	std::string currentHair;

	struct HairOption {
		std::string id;
		std::string displayName;
	};

	std::vector<HairOption> hairOptions;
};

#endif  // IMGUI_HAIR_SELECT_HPP
