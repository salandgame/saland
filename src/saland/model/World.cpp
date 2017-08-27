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

#include "World.hpp"
#include "placeables.hpp"

World::World() {
}


static void AddRectToBody(b2Body* body, float x, float y, float width, float hight) {
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(width/pixel2unit/2.0, hight/pixel2unit/2.0);
	b2FixtureDef myFixtureDef;
	myFixtureDef.shape = &polygonShape;
	body->CreateFixture(&myFixtureDef);
}

/**
 * Adds a static body to the world. Returns a pointer to the body
 * @param world The world to use for the body 
 * @param x X coordiante for the top left corner in pixels
 * @param y Y coordinate for the top left corner in pixels 
 * @param width Width of the rectangle in pixels
 * @param hight Hight of the rectangle in pixels
 * @return A pointer to the body. Valid until the world is destroyed or destroyBody is called.
 */
static b2Body* AddStaticRect(b2World* world, float x, float y, float width, float hight) {
	b2BodyDef myBodyDef;
	myBodyDef.type = b2_staticBody;
	myBodyDef.position.Set(x/pixel2unit+width/pixel2unit/2.0, y/pixel2unit + hight/pixel2unit/2.0 );
	myBodyDef.linearDamping = 1.0f;
	b2Body* body = world->CreateBody(&myBodyDef);
	AddRectToBody(body, x, y, width, hight);
	return body;
}

static void AddStaticTilesToWorld(b2World* world, const sago::tiled::TileMap& tm, const sago::tiled::TileLayer& layer) {
	for (int i = 0; i < tm.height; ++i) {
		for (int j = 0; j < tm.width; ++j) {
			uint32_t gid = sago::tiled::getTileFromLayer(tm, layer, i, j);
			if (gid == 0) {
				continue;
			}
			AddStaticRect(world, i*32.0f, j*32.0f, 32.0f, 32.0f);
		}
	}
}

void World::init(std::shared_ptr<b2World>& world) {
	this->physicsWorld = world;
	std::string tsx_file = sago::GetFileContent("maps/terrain.tsx");
	std::string tmx_file = sago::GetFileContent("maps/sample1.tmx");
	ts = sago::tiled::string2tileset(tsx_file);
	tm = sago::tiled::string2tilemap(tmx_file);
	tm.tileset.alternativeSource = &ts;
	const std::vector<sago::tiled::TileObjectGroup>& object_groups = tm.object_groups;
	for (const auto& group : object_groups) {
		for (const auto& item : group.objects) {
			if (item.isEcplise) {
				continue;
			}
			if (item.x > 0 && item.y > 0 && item.width > 0 && item.height > 0) {
				AddStaticRect(physicsWorld.get(), item.x, item.y, item.width, item.height);
			}
		}
	}
	const std::vector<sago::tiled::TileLayer>& layers = tm.layers;
	for (const auto& layer : layers) {
		if (layer.name != "blocking") {
			continue;
		}
		AddStaticTilesToWorld(physicsWorld.get(), tm, layer);
	}
}

