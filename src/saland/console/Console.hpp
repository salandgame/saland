/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2018 Poul Sander

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

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "../../sago/GameStateInterface.hpp"

class Console : public sago::GameStateInterface {
public:
	Console();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool &processed) override;
	virtual void Update() override;
	Console(const Console& orig) = delete;
	Console& operator=(const Console&) = delete;
	virtual ~Console();
private:
	bool active = true;
};

#endif /* CONSOLE_HPP */

