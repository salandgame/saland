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


#ifndef _HELP_COMMON_HPP
#define _HELP_COMMON_HPP

#include "../sago/SagoTextBox.hpp"
#include "../sago/SagoTextField.hpp"


void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text);
void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text);

void setHelpBoxFont(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text);

#endif  //_HELP_COMMON_HPP
