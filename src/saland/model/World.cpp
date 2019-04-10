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

/**
 * Adds a rext based on pixels to a body.
 * @param body The body to add to
 * @param x X coordiante for the top left corner in pixels
 * @param y Y coordinate for the top left corner in pixels
 * @param width Width of the rectangle in pixels
 * @param hight Hight of the rectangle in pixels
 */
static void AddRectToBody(b2Body* body, float x, float y, float width, float hight) {
	b2Vec2 center;
	center.Set(x/pixel2unit+width/pixel2unit/2.0, y/pixel2unit + hight/pixel2unit/2.0);
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(width/pixel2unit/2.0, hight/pixel2unit/2.0, center, 0.0f);
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
	myBodyDef.position.Set(0,0);
	myBodyDef.linearDamping = 1.0f;
	b2Body* body = world->CreateBody(&myBodyDef);
	AddRectToBody(body, x, y, width, hight);
	return body;
}

static b2Body* AddStaticBody(b2World* world) {
	b2BodyDef myBodyDef;
	myBodyDef.type = b2_staticBody;
	myBodyDef.position.Set(0,0);
	myBodyDef.linearDamping = 1.0f;
	b2Body* body = world->CreateBody(&myBodyDef);
	return body;
}

static b2Body* AddStaticTilesToWorld(b2World* world, const sago::tiled::TileMap& tm, const sago::tiled::TileLayer& layer) {
	b2Body* body = AddStaticBody(world);
	for (int i = 0; i < tm.height; ++i) {
		for (int j = 0; j < tm.width; ++j) {
			uint32_t gid = sago::tiled::getTileFromLayer(tm, layer, i, j);
			if (gid == 0) {
				continue;
			}
			AddRectToBody(body, i*32.0f, j*32.0f, 32.0f, 32.0f);
		}
	}
	return body;
}

void World::init_physics(std::shared_ptr<b2World>& world) {
	for (b2Body* b : managed_bodies) {
		b2Fixture* f = b->GetFixtureList();
		while (f) {
			b2Fixture* next_f = f->GetNext();
			b->DestroyFixture(f);
			f = next_f;
		}
		world->DestroyBody(b);
	}
	managed_bodies.clear();
	const std::vector<sago::tiled::TileObjectGroup>& object_groups = tm.object_groups;
	for (const auto& group : object_groups) {
		for (const auto& item : group.objects) {
			if (item.isEllipse) {
				continue;
			}
			if (item.x > 0 && item.y > 0 && item.width > 0 && item.height > 0) {
				b2Body* bodyAdded = AddStaticRect(physicsWorld.get(), item.x, item.y, item.width, item.height);
				managed_bodies.push_back(bodyAdded);
			}
		}
	}
	const std::vector<sago::tiled::TileLayer>& layers = tm.layers;
	for (const auto& layer : layers) {
		if (layer.name != "blocking") {
			continue;
		}
		b2Body* bodyAdded = AddStaticTilesToWorld(physicsWorld.get(), tm, layer);
		managed_bodies.push_back(bodyAdded);
	}
	{
		//Top left
		b2Body* bodyAdded = AddStaticRect(physicsWorld.get(), 0, -32, (layers.at(0).width/2-5)*32, 32);
		managed_bodies.push_back(bodyAdded);
		//top right
		bodyAdded = AddStaticRect(physicsWorld.get(), (layers.at(0).width/2+5)*32, -32, (layers.at(0).width)*32, 32);
		managed_bodies.push_back(bodyAdded);
		//bottom left
		bodyAdded = AddStaticRect(physicsWorld.get(), 0, (layers.at(0).height*32), (layers.at(0).width/2-5)*32, 32);
		managed_bodies.push_back(bodyAdded);
		//bottom right
		bodyAdded = AddStaticRect(physicsWorld.get(), (layers.at(0).width/2+5)*32, (layers.at(0).height*32), (layers.at(0).width)*32, 32);
		managed_bodies.push_back(bodyAdded);
		bodyAdded = AddStaticRect(physicsWorld.get(), -32, 0, 32, (layers.at(0).height/2-5)*32);
		managed_bodies.push_back(bodyAdded);
		bodyAdded = AddStaticRect(physicsWorld.get(), -32, (layers.at(0).height/2+5)*32, 32, layers.at(0).height*32);
		managed_bodies.push_back(bodyAdded);
		bodyAdded = AddStaticRect(physicsWorld.get(), (layers.at(0).width)*32, 0, 32, (layers.at(0).height/2-5)*32);
		managed_bodies.push_back(bodyAdded);
		bodyAdded = AddStaticRect(physicsWorld.get(), (layers.at(0).width)*32, (layers.at(0).height/2+5)*32, 32, layers.at(0).height*32);
		managed_bodies.push_back(bodyAdded);
	}
}

void World::init(std::shared_ptr<b2World>& world, const std::string& mapFileName) {
	this->physicsWorld = world;
	std::string tmx_file = sago::GetFileContent(mapFileName);
	tm = sago::tiled::string2tilemap(tmx_file);
	for (size_t i = 0; i < tm.tileset.size(); ++i) {
		if (tm.tileset[i].source.length()) {
			std::string tsx_file = sago::GetFileContent("maps/"+tm.tileset[i].source);
			ts = sago::tiled::string2tileset(tsx_file);
			tm.tileset[i].alternativeSource = &ts;
		}
	}
	init_physics(world);
}

void World::init(std::shared_ptr<b2World>& world) {
	init(world, "maps/sample1.tmx");
}

