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
https://github.com/salandgame/saland
===========================================================================
*/

/*
Added to project 5/11-2004
*/

#ifndef READKEYBOARD_HPP
#define READKEYBOARD_HPP

#include "SDL.h"
#include <string>

class ReadKeyboard
{
private:
	int maxLength = 0;
	std::string::iterator position;
	std::string text_string;
	void removeChar();
public:
	ReadKeyboard(void);
	~ReadKeyboard(void);
	explicit ReadKeyboard(const char*);
	int CharsBeforeCursor(); //Where should the cursor be placed?
	void putchar(const std::string& );
	bool cursorLeft();
	bool cursorRight();
	bool emulateBackspace();
	bool ReadKey(const SDL_Event&);
	bool ReadKey(SDL_Keycode); //true if key accepted
	const std::string& GetString(void) const;
};

#endif //READKEYBOARD_HPP