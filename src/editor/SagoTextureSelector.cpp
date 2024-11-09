/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022-2024 Poul Sander

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

#include "SagoTextureSelector.hpp"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <SDL_image.h>
#include "../../sago/SagoMisc.hpp"
#include "../global.hpp"


static void addLinesToCanvas(SDL_Renderer* renderer, SDL_Texture* texture, int xstep = 32, int ystep = 32, int xoffset = 0, int yoffset = 0) {
	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	if (xstep > 0) {
		for (int i = 0; i < width+1; i += xstep) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(i+xoffset, yoffset), ImVec2(i+xoffset, height+yoffset), IM_COL32(255, 0, 0, 255));
		}
	}
	if (ystep > 0) {
		for (int i = 0; i < height+1; i += ystep) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(xoffset, i+yoffset), ImVec2(width+xoffset, i+yoffset), IM_COL32(255, 0, 0, 255));
		}
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

static void addRectableToCanvas(SDL_Renderer* renderer, int topx = 0, int topy = 0, int height = 100, int width = 100, int xoffset = 0, int yoffset = 0) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	topx += xoffset;
	topy += yoffset;
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx, topy), ImVec2(topx+width, topy), IM_COL32(255, 0, 0, 255));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx+width, topy), ImVec2(topx+width, topy+height), IM_COL32(255, 0, 0, 255));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx+width, topy+height), ImVec2(topx, topy+height), IM_COL32(255, 0, 0, 255));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx, topy+height), ImVec2(topx, topy), IM_COL32(255, 0, 0, 255));
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

static void addFolderToList(const std::string& folder, std::vector<std::string>& list, const std::string& filter = "") {
	std::vector<std::string> textures = sago::GetFileList(folder.c_str());
	for (const auto& texture : textures) {
		std::cout << "Texture: " << texture << "\n";
		if (texture.find(".png") == std::string::npos) {
			continue;
		}
		if (filter.empty() || texture.find(filter) != std::string::npos) {
			list.push_back(texture);
		}
	}
}

static std::vector<std::string> populateTree(const std::string& filter = "") {
	std::vector<std::string> textures;
	addFolderToList("textures", textures, filter);
	return textures;
}

std::string remove_file_extension(const std::string& filename) {
	size_t lastindex = filename.find_last_of(".");
	return filename.substr(0, lastindex);
}

void SagoTextureSelector::runSpriteSelectorFrame(SDL_Renderer* target) {
	ImGui::Begin("SpriteList", nullptr, ImGuiWindowFlags_NoCollapse);
	static char filter[256] = "";
	ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));
	ImGui::Separator();
	for (const auto& sprite : sprites) {
		std::string sprite_name = sprite.first;
		if (filter[0] == '\0' || sprite_name.find(filter) != std::string::npos) {
			if (ImGui::Selectable(sprite_name.c_str(), selected_sprite == sprite_name)) {
				selected_sprite = sprite_name;
			}
		}
	}

	ImGui::End();

	ImGui::Begin("SpriteViewer");
	if (selected_sprite.length()) {
		int tex_w, tex_h;
		const SagoSprite& current_sprite = sprites[selected_sprite];
		SDL_Texture* current_texture = globalData.dataHolder->getTexturePtr(current_sprite.texture);
		SDL_QueryTexture(current_texture, nullptr, nullptr, &tex_w, &tex_h);
		float sprite_w = current_sprite.width;
		float sprite_h = current_sprite.height;
		float topx = current_sprite.topx;
		float topy = current_sprite.topy;
		ImGui::Text("Size: %d x %d", tex_w, tex_h);
		ImGui::BeginChild("Test");
		ImVec2 p = ImGui::GetCursorScreenPos();

		// Normalized coordinates of pixel (10,10) in a 256x256 texture.
		ImVec2 uv0 = ImVec2(topx / tex_h, topy / tex_w);

		// Normalized coordinates of pixel (110,210) in a 256x256 texture.
		ImVec2 uv1 = ImVec2( (topx+sprite_h) / tex_h, (topy+sprite_w) / tex_w);


		ImGui::Image((ImTextureID)(intptr_t)current_texture, ImVec2((float)sprite_w, (float)sprite_h), uv0, uv1);
		ImGui::EndChild();
	}
	ImGui::End();

	ImGui::Begin("SpriteTexture");
	if (selected_sprite.length() && sprites[selected_sprite].texture.length()) {
		int tex_w, tex_h;
		const SagoSprite& current_sprite = sprites[selected_sprite];
		SDL_Texture* current_texture = globalData.dataHolder->getTexturePtr(current_sprite.texture);
		SDL_QueryTexture(current_texture, nullptr, nullptr, &tex_w, &tex_h);
		ImGui::Text("Size: %d x %d", tex_w, tex_h);
		ImGui::BeginChild("Test");
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::Image((ImTextureID)(intptr_t)current_texture, ImVec2((float)tex_w, (float)tex_h));
		addRectableToCanvas(target, current_sprite.topx, current_sprite.topy, current_sprite.height, current_sprite.width,  p.x, p.y);
		ImGui::EndChild();
	}
	ImGui::End();
}

void SagoTextureSelector::runTextureSelectorFrame(SDL_Renderer* target) {
	ImGui::Begin("TextureList", nullptr, ImGuiWindowFlags_NoCollapse);
	static char filter[256] = "";
	ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));
	ImGui::Separator();
	for (const auto& texture : textures) {
		if (filter[0] == '\0' || texture.find(filter) != std::string::npos) {
			if (ImGui::Selectable(texture.c_str(), selected_texture == texture)) {
				selected_texture = texture;
			}
		}
	}

	ImGui::End();

	ImGui::Begin("TextureViewer");
	if (selected_texture.length()) {
		int tex_w, tex_h;
		SDL_Texture* current_texture = globalData.dataHolder->getTexturePtr(remove_file_extension(selected_texture));
		SDL_QueryTexture(current_texture, nullptr, nullptr, &tex_w, &tex_h);
		ImGui::Text("Size: %d x %d", tex_w, tex_h);
		ImGui::BeginChild("Test");
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::Image((ImTextureID)(intptr_t)current_texture, ImVec2((float)tex_w, (float)tex_h));
		addLinesToCanvas(target, current_texture, 32, 32, p.x, p.y);
		ImGui::EndChild();
	}
	ImGui::End();

}



SagoTextureSelector::SagoTextureSelector() {
}

SagoTextureSelector::~SagoTextureSelector() {
}

bool SagoTextureSelector::IsActive() {
	return isActive;
}

void SagoTextureSelector::ProcessInput(const SDL_Event& event, bool &processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void SagoTextureSelector::Draw(SDL_Renderer* target) {
	runTextureSelectorFrame(target);
	runSpriteSelectorFrame(target);
}

void SagoTextureSelector::Update() {
}

void SagoTextureSelector::Init() {
	textures = populateTree();
	sprites = LoadSprites();
}