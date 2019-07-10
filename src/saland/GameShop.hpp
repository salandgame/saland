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

#ifndef GAMESHOP_HPP
#define GAMESHOP_HPP

#include "../sago/GameStateInterface.hpp"
#include "../sago/SagoTextField.hpp"
#include <memory>
#include <vector>

struct ShopInfo {
	std::string id = "Default";
	std::string name = "Default shop";
	std::vector<std::string> items;
};

class GameShop : public sago::GameStateInterface {
public:
	GameShop();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool &processed) override;
	virtual void Update() override;
	GameShop(const GameShop&) = delete;
	virtual ~GameShop();
private:
	struct GameShopImpl;
	std::unique_ptr<GameShopImpl> data;
};

#endif /* GAMEAREA_HPP */