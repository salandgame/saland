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

#include "GameDraw.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

static void Draw(SDL_Renderer* target, SDL_Texture* t, int x, int y, const SDL_Rect& part) {
	SDL_Rect pos = {};
	pos.x = x;
	pos.y = y;
	pos.w = 32;
	pos.h = 32;
	SDL_RenderCopy(target, t, &part, &pos);
}

void DrawOuterBorder(SDL_Renderer* renderer, SDL_Texture* texture, const sago::tiled::TileMap& tm, int topx, int topy, uint32 outerTile) {
	for (int i = -1; i < tm.width + 1; ++i) {
		if (i >= tm.width/2-5 && i < tm.width/2+5) {
			continue;
		}
		SDL_Rect part{};
		getTextureLocationFromGid(tm, outerTile, nullptr, &part.x, &part.y, &part.w, &part.h);
		Draw(renderer, texture, 32 * i - topx, -32 - topy, part);
		Draw(renderer, texture, 32 * i - topx, 32 * tm.height - topy, part);
	}
	for (int i = 0; i < tm.height; ++i) {
		if (i >= tm.height/2-5 && i < tm.height/2+5) {
			continue;
		}
		SDL_Rect part{};
		getTextureLocationFromGid(tm, outerTile, nullptr, &part.x, &part.y, &part.w, &part.h);
		Draw(renderer, texture, 32 * tm.width - topx, 32*i - topy, part);
		Draw(renderer, texture, -32 - topx, 32*i - topy, part);
	}
}

void DrawLayer(SDL_Renderer* renderer, SDL_Texture* texture, const sago::tiled::TileMap& tm, size_t layer, int topx, int topy) {
	for (int i = 0; i < tm.height; ++i) {
		for (int j = 0; j < tm.width; ++j) {
			uint32_t gid = sago::tiled::getTileFromLayer(tm, tm.layers.at(layer), i, j);
			if (gid == 0) {
				continue;
			}
			SDL_Rect part{};
			getTextureLocationFromGid(tm, gid, nullptr, &part.x, &part.y, &part.w, &part.h);
			Draw(renderer, texture, 32 * i - topx, 32 * j - topy, part);
		}
	}
}

void DrawOjbectGroup(SDL_Renderer* renderer, const sago::tiled::TileMap& tm, size_t object_group, int topx, int topy) {
	const sago::tiled::TileObjectGroup& group = tm.object_groups.at(object_group);
	for (const sago::tiled::TileObject& o : group.objects) {
		if (o.isEllipse) {
			ellipseRGBA(renderer, (o.x + o.width / 2) - topx, (o.y + o.height / 2) - topy, o.width / 2, o.height / 2, 255, 255, 0, 255);
		} else if (o.polygon_points.size() > 0) {
			for (size_t i = 0; i < o.polygon_points.size(); ++i) {
				std::pair<int, int> first = o.polygon_points.at(i);
				std::pair<int, int> second = (i + 1 < o.polygon_points.size()) ? o.polygon_points.at(i + 1) : o.polygon_points.at(0);
				lineRGBA(renderer, first.first + o.x - topx, first.second + o.y - topy, second.first + o.x - topx, second.second + o.y - topy, 255, 255, 0, 255);
			}
		} else {
			rectangleRGBA(renderer, o.x - topx, o.y - topy,
				o.x + o.width - topx, o.y + o.height - topy, 255, 255, 0, 255);
		}
	}
}

void DrawMiscEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const MiscItem *entity, float time,
	int offsetX, int offsetY, bool drawCollision) {
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
	const sago::SagoSprite &mySprite = sHolder->GetSprite(entity->sprite);
	mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
}

void DrawHumanEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Human *entity, float time, int offsetX, int offsetY, bool drawCollision) {
	std::string animation = "standing";
	bool relativeAnimation = false;
	float relativeAnimationState = 0.0f;
	if (entity->moving) {
		animation = "walkcycle";
	}
	if (entity->castTimeRemaining) {
		animation = "spellcast";
		relativeAnimation = true;
		relativeAnimationState = 1.0f - (entity->castTimeRemaining / entity->castTime);
	}
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
	const sago::SagoSprite &mySprite = sHolder->GetSprite(entity->race + "_" + animation + "_" + std::string(1, entity->direction));
	if (relativeAnimation) {
		mySprite.DrawProgressive(target, relativeAnimationState, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
	} else {
		mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
	}
	if (entity->hair.length() > 0) {
		std::string hairAnimation = animation;
		if (hairAnimation == "spellcast") {
			hairAnimation = "standing";
		}
		const sago::SagoSprite &myHair = sHolder->GetSprite(entity->race + "_"+hairAnimation+"_"+entity->hair+"_"+std::string(1,entity->direction));
		myHair.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
	}
	if (entity->pants.length() > 0) {
		const sago::SagoSprite &myPants = sHolder->GetSprite(entity->race + "_"+animation+"_"+entity->pants+"_"+std::string(1,entity->direction));
		if (relativeAnimation) {
			myPants.DrawProgressive(target, relativeAnimationState, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
		}
		else {
			myPants.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
		}
	}
}

void DrawMonster(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Monster *entity, float time, int offsetX, int offsetY, bool drawCollision) {
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
	const sago::SagoSprite &mySprite = sHolder->GetSprite(entity->race + "_" + std::string(1, entity->direction));
	mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY);
}


void DrawProjectile(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Projectile *entity, float time, int offsetX, int offsetY, bool drawCollision) {
	(void)sHolder;
	(void)time;
	if (drawCollision) {
		circleRGBA(target,
			entity->X - offsetX, entity->Y - offsetY, entity->Radius,
			255, 255, 0, 255);
	}
}
