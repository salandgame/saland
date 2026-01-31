/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2026 Poul Sander

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

#include "GameInventoryState.hpp"
#include "GameItems.hpp"
#include "globals.hpp"
#include "../global.hpp"
#include "imgui.h"
#include <algorithm>

struct GameInventoryState::GameInventoryStateImpl {
	bool active = false;
	int current_tab = 0;
};

GameInventoryState::GameInventoryState() {
	data.reset(new GameInventoryState::GameInventoryStateImpl());
}

GameInventoryState::~GameInventoryState() {
}

void GameInventoryState::Activate() {
	data->active = true;
}

void GameInventoryState::Deactivate() {
	data->active = false;
}

bool GameInventoryState::IsActive() {
	return data->active;
}

void GameInventoryState::Draw(SDL_Renderer*) {
	if (!data->active) {
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Inventory", &data->active)) {
		if (ImGui::BeginTabBar("InventoryTabs")) {
			// Tab 1: All Items
			if (ImGui::BeginTabItem("All Items")) {
				data->current_tab = 0;

				ImGui::Text("Your Items:");
				ImGui::Separator();

				if (ImGui::BeginChild("ItemsList", ImVec2(0, 0), true)) {
					for (const auto& item : globalData.player.item_inventory) {
						ImGui::Text("%s: %d", item.first.c_str(), item.second);
					}
					if (globalData.player.item_inventory.empty()) {
						ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No items in inventory");
					}
				}
				ImGui::EndChild();
				
				ImGui::EndTabItem();
			}

			// Tab 2: Equipment
			if (ImGui::BeginTabItem("Equipment")) {
				data->current_tab = 1;
				ImGui::Text("Equipped Items:");
				ImGui::Separator();
				// Display currently equipped items
				ImGui::Text("Currently Equipped:");
				for (const std::string& equipped : globalData.player.equipped_items) {
					ImGui::BulletText("%s", equipped.c_str());
				}
				if (globalData.player.equipped_items.empty()) {
					ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Nothing equipped");
				}
				ImGui::Separator();
				ImGui::Spacing();
				// Upper body section
				ImGui::Text("Upper Body:");
				std::string current_top = "";
				for (const std::string& equipped : globalData.player.equipped_items) {
					if (itemExists(equipped)) {
						const ItemDef& item = getItem(equipped);
						if (!item.armor_upper_body.empty()) {
							current_top = equipped;
							break;
						}
					}
				}
				if (!current_top.empty()) {
					ImGui::Text("Equipped: %s", current_top.c_str());
					if (ImGui::Button("Unequip##top")) {
						auto it = std::find(globalData.player.equipped_items.begin(), 
						                   globalData.player.equipped_items.end(), current_top);
						if (it != globalData.player.equipped_items.end()) {
							globalData.player.equipped_items.erase(it);
							ApplyEquippedItems(globalData.player);
							PlayerSave();
						}
					}
				} else {
					ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Nothing equipped");
				}
				ImGui::Text("Available:");
				for (const auto& item : globalData.player.item_inventory) {
					if (itemExists(item.first)) {
						const ItemDef& itemDef = getItem(item.first);
						if (!itemDef.armor_upper_body.empty()) {
							bool already_equipped = (item.first == current_top);
							if (!already_equipped) {
								if (ImGui::Button(("Equip " + item.first).c_str())) {
									// Remove any currently equipped upper body item
									if (!current_top.empty()) {
										auto it = std::find(globalData.player.equipped_items.begin(), 
										                   globalData.player.equipped_items.end(), current_top);
										if (it != globalData.player.equipped_items.end()) {
											globalData.player.equipped_items.erase(it);
										}
									}
									// Equip the new item
									globalData.player.equipped_items.push_back(item.first);
									ApplyEquippedItems(globalData.player);
									PlayerSave();
								}
							}
						}
					}
				}

				ImGui::Separator();
				ImGui::Spacing();

				// Lower body section
				ImGui::Text("Lower Body:");
				std::string current_bottom = "";
				for (const std::string& equipped : globalData.player.equipped_items) {
					if (itemExists(equipped)) {
						const ItemDef& item = getItem(equipped);
						if (!item.armor_lower_body.empty()) {
							current_bottom = equipped;
							break;
						}
					}
				}

				if (!current_bottom.empty()) {
					ImGui::Text("Equipped: %s", current_bottom.c_str());
					if (ImGui::Button("Unequip##bottom")) {
						auto it = std::find(globalData.player.equipped_items.begin(), 
						                   globalData.player.equipped_items.end(), current_bottom);
						if (it != globalData.player.equipped_items.end()) {
							globalData.player.equipped_items.erase(it);
							ApplyEquippedItems(globalData.player);
							PlayerSave();
						}
					}
				} else {
					ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Nothing equipped");
				}

				ImGui::Text("Available:");
				for (const auto& item : globalData.player.item_inventory) {
					if (itemExists(item.first)) {
						const ItemDef& itemDef = getItem(item.first);
						if (!itemDef.armor_lower_body.empty()) {
							bool already_equipped = (item.first == current_bottom);
							if (!already_equipped) {
								if (ImGui::Button(("Equip " + item.first).c_str())) {
									// Remove any currently equipped lower body item
									if (!current_bottom.empty()) {
										auto it = std::find(globalData.player.equipped_items.begin(), 
										                   globalData.player.equipped_items.end(), current_bottom);
										if (it != globalData.player.equipped_items.end()) {
											globalData.player.equipped_items.erase(it);
										}
									}
									// Equip the new item
									globalData.player.equipped_items.push_back(item.first);
									ApplyEquippedItems(globalData.player);
									PlayerSave();
								}
							}
						}
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void GameInventoryState::ProcessInput(const SDL_Event& event, bool& processed) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_i) {
			data->active = !data->active;
			processed = true;
		}
		if (event.key.keysym.sym == SDLK_ESCAPE && data->active) {
			data->active = false;
			processed = true;
		}
	}
}

void GameInventoryState::Update() {
}
