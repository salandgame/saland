/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2019 Poul Sander

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

#include "globals.hpp"
#include "GameShop.hpp"

struct GameShop::GameShopImpl {
	bool active = true;
	std::string name = "Shop!!";
	sago::SagoTextField headerField;
	sago::SagoTextField goldField;
};


GameShop::GameShop() {
	data.reset(new GameShop::GameShopImpl());
	data->headerField.SetHolder(globalData.dataHolder);
	data->headerField.SetFontSize(20);
	data->headerField.SetText(data->name);
	data->goldField.SetHolder(globalData.dataHolder);
	data->goldField.SetFontSize(16);
}

bool GameShop::IsActive() {
	return data->active;
}

void GameShop::Draw(SDL_Renderer* target) {
	data->headerField.Draw(target, globalData.xsize/2, 4, sago::SagoTextField::Alignment::center);
	data->goldField.SetText(std::string("Gold: ")+std::to_string(globalData.player.item_inventory["gold"]));
	data->goldField.Draw(target, globalData.xsize-1, globalData.ysize-1, sago::SagoTextField::Alignment::right, sago::SagoTextField::VerticalAlignment::bottom);
}

void GameShop::ProcessInput(const SDL_Event& event, bool &) {
	if (event.type == SDL_KEYDOWN) {
		data->active = false;
	}
}

void GameShop::Update() {

}

GameShop::~GameShop() {

}