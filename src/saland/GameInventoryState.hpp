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

#ifndef GAME_INVENTORY_STATE_HPP
#define GAME_INVENTORY_STATE_HPP

#include <memory>
#include "../sago/GameStateInterface.hpp"

class GameInventoryState : public sago::GameStateInterface {
public:
	GameInventoryState();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool& processed) override;
	virtual void Update() override;
	virtual ~GameInventoryState();
	void Activate();
	void Deactivate();
private:
	struct GameInventoryStateImpl;
	std::unique_ptr<GameInventoryStateImpl> data;
};

#endif /* GAME_INVENTORY_STATE_HPP */
