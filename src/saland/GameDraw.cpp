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
#include <cmath>
#include "../sago/SagoTextField.hpp"

struct TextCache {
	sago::SagoTextField* getLabel(const std::string& text);
private:
	std::map<std::string, std::shared_ptr<sago::SagoTextField> > labels;
};

sago::SagoTextField* TextCache::getLabel(const std::string& text) {
	const auto& theLabel = labels.find(text);
	if (theLabel != labels.end()) {
		return labels[text].get();
	}
	std::shared_ptr<sago::SagoTextField> newField = std::make_shared<sago::SagoTextField>();
	newField->SetHolder(&globalData.spriteHolder->GetDataHolder());
	newField->SetFont("freeserif");
	newField->SetFontSize(12);
	newField->SetOutline(0, {255,255,0,255});
	newField->SetText(text);
	labels[text] = newField;
	return labels[text].get();
}


TextCache textCache;


static void Draw(SDL_Renderer* target, SDL_Texture* t, int x, int y, const SDL_Rect& part, sago::SagoLogicalResize* resize = nullptr) {
	SDL_Rect pos = {};
	pos.x = x;
	pos.y = y;
	pos.w = 32;
	pos.h = 32;
	if (resize) {
		resize->LogicalToPhysical(pos);
	}
	SDL_RenderCopy(target, t, &part, &pos);
}

static void DrawCollision(SDL_Renderer* target, const Placeable* entity, int offsetX, int offsetY, bool drawCollision, sago::SagoLogicalResize* resize = nullptr) {
	if (drawCollision) {
		int x = entity->X - offsetX;
		int y = entity->Y - offsetY;
		int r = entity->Radius;
		if (resize) {
			resize->LogicalToPhysical(&x, &y);
			int temp_r = r;
			resize->LogicalToPhysical(&temp_r, nullptr);
			r = temp_r;
		}
		circleRGBA(target,
		           x, y, r,
		           255, 255, 0, 255);
		textCache.getLabel(entity->id)->Draw(target, x + r + 4, y, sago::SagoTextField::Alignment::left, sago::SagoTextField::VerticalAlignment::center, resize);
	}
}

void DrawOuterBorder(SDL_Renderer* renderer, SDL_Texture* texture, const sago::tiled::TileMap& tm, int topx, int topy, uint32 outerTile, sago::SagoLogicalResize* resize) {
	for (int i = -1; i < tm.width + 1; ++i) {
		if (i >= tm.width/2-5 && i < tm.width/2+5) {
			continue;
		}
		SDL_Rect part{};
		getTextureLocationFromGid(tm, outerTile, nullptr, &part.x, &part.y, &part.w, &part.h);
		Draw(renderer, texture, 32 * i - topx, -32 - topy, part, resize);
		Draw(renderer, texture, 32 * i - topx, 32 * tm.height - topy, part, resize);
	}
	for (int i = 0; i < tm.height; ++i) {
		if (i >= tm.height/2-5 && i < tm.height/2+5) {
			continue;
		}
		SDL_Rect part{};
		getTextureLocationFromGid(tm, outerTile, nullptr, &part.x, &part.y, &part.w, &part.h);
		Draw(renderer, texture, 32 * tm.width - topx, 32*i - topy, part, resize);
		Draw(renderer, texture, -32 - topx, 32*i - topy, part, resize);
	}
}

void DrawTile(SDL_Renderer* renderer, sago::SagoSpriteHolder* sHolder, const sago::tiled::TileMap& tm, uint32_t gid, int x, int y, sago::SagoLogicalResize* resize) {
	SDL_Rect part{};
	std::string imageFile;
	getTextureLocationFromGid(tm, gid, &imageFile, &part.x, &part.y, &part.w, &part.h);
	imageFile = imageFile.substr(12);
	imageFile = imageFile.substr(0, imageFile.length()-4);
	SDL_Texture* texture = sHolder->GetDataHolder().getTexturePtr(imageFile);
	Draw(renderer, texture, x, y, part, resize);
}

void DrawLayer(SDL_Renderer* renderer, sago::SagoSpriteHolder* sHolder, const sago::tiled::TileMap& tm, size_t layer, int topx, int topy, sago::SagoLogicalResize* resize) {
	int startX = topx/32;
	int startY = topy/32;
	if (startX < 0) {
		startX = 0;
	}
	if (startY < 0) {
		startY = 0;
	}
	// Use logical size for drawing range
	int viewWidth = resize ? 1920 : globalData.xsize;
	int viewHeight = resize ? 1080 : globalData.ysize;
	for (int i = startX; i < tm.width && i < (topx+viewWidth)/32+1; ++i) {
		for (int j = startY; j < tm.height && j < (topy+viewHeight)/32+1; ++j) {
			uint32_t gid = sago::tiled::getTileFromLayer(tm, tm.layers.at(layer), i, j);
			if (gid == 0) {
				continue;
			}
			DrawTile(renderer, sHolder, tm, gid, 32 * i - topx, 32 * j - topy, resize);
		}
	}
}

void DrawOjbectGroup(SDL_Renderer* renderer, const sago::tiled::TileMap& tm, size_t object_group, int topx, int topy, sago::SagoLogicalResize* resize) {
	const sago::tiled::TileObjectGroup& group = tm.object_groups.at(object_group);
	for (const sago::tiled::TileObject& o : group.objects) {
		if (o.isEllipse) {
			int cx = (o.x + o.width / 2) - topx;
			int cy = (o.y + o.height / 2) - topy;
			int rx = o.width / 2;
			int ry = o.height / 2;
			if (resize) {
				resize->LogicalToPhysical(&cx, &cy);
				int temp_rx = rx, temp_ry = ry;
				resize->LogicalToPhysical(&temp_rx, &temp_ry);
				rx = temp_rx;
				ry = temp_ry;
			}
			ellipseRGBA(renderer, cx, cy, rx, ry, 255, 255, 0, 255);
		}
		else if (o.polygon_points.size() > 0) {
			for (size_t i = 0; i < o.polygon_points.size(); ++i) {
				std::pair<int, int> first = o.polygon_points.at(i);
				std::pair<int, int> second = (i + 1 < o.polygon_points.size()) ? o.polygon_points.at(i + 1) : o.polygon_points.at(0);
				int x1 = first.first + o.x - topx;
				int y1 = first.second + o.y - topy;
				int x2 = second.first + o.x - topx;
				int y2 = second.second + o.y - topy;
				if (resize) {
					resize->LogicalToPhysical(&x1, &y1);
					resize->LogicalToPhysical(&x2, &y2);
				}
				lineRGBA(renderer, x1, y1, x2, y2, 255, 255, 0, 255);
			}
		}
		else {
			if (globalData.debugDrawProtectedAreas) {
				int x1 = o.x - topx;
				int y1 = o.y - topy;
				int x2 = o.x + o.width - topx;
				int y2 = o.y + o.height - topy;
				if (resize) {
					resize->LogicalToPhysical(&x1, &y1);
					resize->LogicalToPhysical(&x2, &y2);
				}
				rectangleRGBA(renderer, x1, y1, x2, y2, 255, 255, 0, 255);
			}
		}
	}
}

void DrawMiscEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const MiscItem* entity, float time,
                    int offsetX, int offsetY, bool drawCollision, sago::SagoLogicalResize* resize) {
	DrawCollision(target, entity, offsetX, offsetY, drawCollision, resize);
	const sago::SagoSprite& mySprite = sHolder->GetSprite(entity->sprite);
	mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY, resize);
	if (entity->sprite2[0]) {
		const sago::SagoSprite& mySprite2 = sHolder->GetSprite(entity->sprite2);
		mySprite2.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY, resize);
	}
}

void DrawHumanEntity(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Human* entity, float time, int offsetX, int offsetY, bool drawCollision, sago::SagoLogicalResize* resize) {
	std::string animation = "standing";
	bool relativeAnimation = false;
	float relativeAnimationState = 0.0f;
	if (entity->moving) {
		animation = "walkcycle";
	}
	if (entity->castTimeRemaining) {
		animation = entity->animation;
		relativeAnimation = true;
		relativeAnimationState = 1.0f - (entity->castTimeRemaining / entity->castTime);
	}
	if (entity->diedAt) {
		animation = "hurt";
		relativeAnimation = true;
		relativeAnimationState = 0.9f;
	}
	int x = std::round(entity->X) - offsetX;
	int y = std::round(entity->Y) - offsetY;
	DrawCollision(target, entity, offsetX, offsetY, drawCollision, resize);
	const sago::SagoSprite& mySprite = sHolder->GetSprite(entity->race + "_" + animation + "_" + std::string(1, entity->direction));
	if (relativeAnimation) {
		mySprite.DrawProgressive(target, relativeAnimationState, x, y, resize);
	}
	else {
		mySprite.Draw(target, time, x, y, resize);
	}
	if (entity->pants.length() > 0) {
		const sago::SagoSprite& myPants = sHolder->GetSprite(entity->race + "_"+animation+"_"+entity->pants+"_"+std::string(1,entity->direction));
		if (relativeAnimation) {
			myPants.DrawProgressive(target, relativeAnimationState, x, y, resize);
		}
		else {
			myPants.Draw(target, time, x, y, resize);
		}
	}
	if (entity->top.length() > 0) {
		const sago::SagoSprite& myTop = sHolder->GetSprite(entity->race + "_"+animation+"_"+entity->top+"_"+std::string(1,entity->direction));
		if (relativeAnimation) {
			myTop.DrawProgressive(target, relativeAnimationState, x, y, resize);
		}
		else {
			myTop.Draw(target, time, x, y, resize);
		}
	}
	if (entity->hair.length() > 0) {
		std::string hairAnimation = animation;
		if (hairAnimation == "spellcast") {
			hairAnimation = "standing";
		}
		const sago::SagoSprite& myHair = sHolder->GetSprite(entity->race + "_"+hairAnimation+"_"+entity->hair+"_"+std::string(1,entity->direction));
		if (relativeAnimation) {
			myHair.DrawProgressive(target, relativeAnimationState, x, y, resize);
		}
		else {
			myHair.Draw(target, time, x, y, resize);
		}
	}
	if (entity->weapon.length() > 0) {
		const sago::SagoSprite& myWeapon = sHolder->GetSprite("human_"+animation+"_"+entity->weapon+"_"+std::string(1,entity->direction));
		if (relativeAnimation) {
			myWeapon.DrawProgressive(target, relativeAnimationState, x, y, resize);
		}
		else {
			myWeapon.Draw(target, time, x, y, resize);
		}
	}
}

void DrawMonster(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Monster* entity, float time, int offsetX, int offsetY, bool drawCollision, sago::SagoLogicalResize* resize) {
	DrawCollision(target, entity, offsetX, offsetY, drawCollision, resize);
	const sago::SagoSprite& mySprite = sHolder->GetSprite(entity->race + "_" + std::string(1, entity->direction));
	mySprite.Draw(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY, resize);
}


void DrawProjectile(SDL_Renderer* target, sago::SagoSpriteHolder* sHolder, const Projectile* entity, float time, int offsetX, int offsetY, bool drawCollision, sago::SagoLogicalResize* resize) {
	(void)sHolder;
	(void)time;
	if (entity->sprite.length()) {
		const sago::SagoSprite& mySprite = sHolder->GetSprite(entity->sprite);
		double angleRadian = 0.0;
		if (entity->directionY || entity->directionX) {
			//atan2 is defined if either of directionY or directionX is not zero
			angleRadian = atan2(entity->directionY, entity->directionX)-M_PI/2.0;
		}
		mySprite.DrawRotated(target, time, std::round(entity->X) - offsetX, std::round(entity->Y) - offsetY, angleRadian, resize);
	}
	DrawCollision(target, entity, offsetX, offsetY, drawCollision, resize);
}

static void DrawRect(SDL_Renderer* target, int topx, int topy, int height, int width, const std::string& name, sago::SagoLogicalResize* resize = nullptr) {
	const int size = 32;
	SDL_Rect bounds_ns = {topx, topy+size, width, height-2*size};  //bounds for south
	SDL_Rect bounds_e = {topx, topy, width-size, height};
	const sago::SagoSprite& n = globalData.spriteHolder->GetSprite(name+"n");
	const sago::SagoSprite& s = globalData.spriteHolder->GetSprite(name+"s");
	const sago::SagoSprite& e = globalData.spriteHolder->GetSprite(name+"e");
	const sago::SagoSprite& w = globalData.spriteHolder->GetSprite(name+"w");
	const sago::SagoSprite& fill = globalData.spriteHolder->GetSprite(name+"fill");
	for (int i = 1; i < width/size; ++i) {
		n.DrawBounded(target, SDL_GetTicks(), topx+i*size, topy, bounds_e, resize);
		for (int j = 1; j < height/size; ++j) {
			w.DrawBounded(target, SDL_GetTicks(), topx, topy+j*size, bounds_ns, resize);
			fill.Draw(target, SDL_GetTicks(),topx+i*size, topy+j*size, resize);
			e.DrawBounded(target, SDL_GetTicks(), topx+width-size, topy+j*size, bounds_ns, resize);
		}
		s.DrawBounded(target, SDL_GetTicks(), topx+i*size, topy+height-size, bounds_e, resize);
	}
	//Corners
	const sago::SagoSprite& nw = globalData.spriteHolder->GetSprite(name+"nw");
	const sago::SagoSprite& ne = globalData.spriteHolder->GetSprite(name+"ne");
	const sago::SagoSprite& se = globalData.spriteHolder->GetSprite(name+"se");
	const sago::SagoSprite& sw = globalData.spriteHolder->GetSprite(name+"sw");
	nw.Draw(target, SDL_GetTicks(), topx, topy, resize);
	ne.Draw(target, SDL_GetTicks(), topx+width-size, topy, resize);
	se.Draw(target, SDL_GetTicks(), topx+width-size, topy+height-size, resize);
	sw.Draw(target, SDL_GetTicks(), topx, topy+height-size, resize);
}

void DrawRectWhite(SDL_Renderer* target, int topx, int topy, int height, int width, sago::SagoLogicalResize* resize) {
	std::string name = "ui_rect_white_";
	DrawRect(target, topx, topy, height, width, name, resize);
}

void DrawRectYellow(SDL_Renderer* target, int topx, int topy, int height, int width, sago::SagoLogicalResize* resize) {
	std::string name = "ui_rect_yellow_";
	DrawRect(target, topx, topy, height, width, name, resize);
}