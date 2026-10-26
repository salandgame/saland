/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2021 Poul Sander

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

#pragma once

#include <string>

/**
 * Prints the name and size of every known prefab to stdout.
 */
void ListPrefabs();

/**
 * Renders the named prefab (or every prefab if name is "all") to "<outputDir>/prefab_<name>.png".
 * Requires globalData.screen and globalData.spriteHolder to be set up.
 * Returns 0 on success and 1 if any prefab could not be rendered.
 */
int RenderPrefabsToPng(const std::string& name, const std::string& outputDir);
