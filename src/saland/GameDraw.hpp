/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2017 Poul Sander

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

#ifndef GAMEDRAW_HPP
#define GAMEDRAW_HPP

#include "globals.hpp"
#include "model/placeables.hpp"
#include "model/World.hpp"
#include "SDL.h"

void DrawOuterBorder(SDL_Renderer* renderer, SDL_Texture* texture, const sago::tiled::TileMap& tm, int topx, int topy, uint32 outerTile);
void DrawLayer(SDL_Renderer* renderer, sago::SagoSpriteHolder* sHolder, const sago::tiled::TileMap& tm, size_t layer, int topx, int topy);
void DrawOjbectGroup(SDL_Renderer* renderer, const sago::tiled::TileMap& tm, size_t object_group, int topx, int topy);
void DrawMiscEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const MiscItem *entity, float time,
	int offsetX, int offsetY, bool drawCollision);
void DrawHumanEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Human *entity, float time, int offsetX, int offsetY, bool drawCollision);
void DrawMonster(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Monster *entity, float time, int offsetX, int offsetY, bool drawCollision);
void DrawProjectile(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Projectile *entity, float time, int offsetX, int offsetY, bool drawCollision);

void DrawRectWhite(SDL_Renderer* target, int topx, int topy, int height, int width);
void DrawRectYellow(SDL_Renderer* target, int topx, int topy, int height, int width);
void DrawTile(SDL_Renderer* renderer, sago::SagoSpriteHolder* sHolder, const sago::tiled::TileMap& tm, uint32_t gid, int x, int y);

#endif /* GAMEDRAW_HPP */

