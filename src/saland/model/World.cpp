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

void fill_blocking_tiles(std::vector<bool>& output, const sago::tiled::TileMap& tm, const sago::tiled::TileLayer& layer) {
	output.resize(tm.height*tm.width);
	for (int i = 0; i < tm.height; ++i) {
		for (int j = 0; j < tm.width; ++j) {
			uint32_t gid = sago::tiled::getTileFromLayer(tm, layer, i, j);
			if (gid == 0) {
				continue;
			}
			output[i+j*tm.width] = true;
		}
	}
}

void destroyBodyWithFixtures(b2World* world, b2Body*& bodyToDestroy) {
	b2Fixture* f = bodyToDestroy->GetFixtureList();
	while (f) {
		b2Fixture* next_f = f->GetNext();
		bodyToDestroy->DestroyFixture(f);
		f = next_f;
	}
	world->DestroyBody(bodyToDestroy);
}

void World::init_physics(std::shared_ptr<b2World>& world) {
	for (b2Body* b : managed_bodies) {
		destroyBodyWithFixtures(world.get(), b);
		/*b2Fixture* f = b->GetFixtureList();
		while (f) {
			b2Fixture* next_f = f->GetNext();
			b->DestroyFixture(f);
			f = next_f;
		}
		world->DestroyBody(b);*/
	}
	managed_bodies.clear();
	const std::vector<sago::tiled::TileObjectGroup>& object_groups = tm.object_groups;
	for (const auto& group : object_groups) {
		for (const auto& item : group.objects) {
			if (item.isEllipse) {
				continue;
			}
			if (item.x > 0 && item.y > 0 && item.width > 0 && item.height > 0 && item.type == "blocking") {
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
		fill_blocking_tiles(blocking_tiles, tm, layer);
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


static size_t append_layer(sago::tiled::TileMap& tm, const char* name) {
	sago::tiled::TileLayer t = createEmptyLayerForMap(tm);
	t.name = name;
	tm.layers.push_back(t);
	return tm.layers.size()-1;
}

static void object_group_add_group_if_not_exist(sago::tiled::TileMap& tm, const char* name) {
	std::vector<sago::tiled::TileObjectGroup>& object_groups = tm.object_groups;
	for (const auto& group : object_groups) {
		if (group.name == name) {
			std::cout << "Already contains group \"" << name << "\"\n";
			return;
		}
	}
	sago::tiled::TileObjectGroup tog;
	tog.name = name;
	object_groups.push_back(tog);
}

/**
 * find the layer "layer" and move it up until it is after "layer_to_place_after"
 * */
static void bubble_layer_after(sago::tiled::TileMap& tm, const char* layer, const char* layer_to_place_after) {
	bool bubbeling = false;
	for (size_t i = 0; i < tm.layers.size(); ++i) {
		std::string layerName = tm.layers[i].name;
		if (bubbeling) {
			std::swap(tm.layers.at(i-1), tm.layers[i]);
		}
		if (layerName == layer) {
			bubbeling = true;
		}
		if (layerName == layer_to_place_after) {
			return;
		}
	}
}

static void init_tilemap(sago::tiled::TileMap& tm, int& blockingLayer, int& blockingLayer_overlay_1) {
	blockingLayer = -1;
	blockingLayer_overlay_1 = -1;
	int prefab_layer_ground = -1;
	int prefab_blocking_2 = -1;
	int prefab_layer_overlay = -1;
	for (size_t i=0; i < tm.layers.size(); ++i) {
		if (tm.layers[i].name == "blocking") {
			blockingLayer = i;
		}
		if (tm.layers[i].name == "blocking_overlay_1") {
			blockingLayer_overlay_1 = i;
		}
		if (tm.layers[i].name == "prefab_ground_1") {
			prefab_layer_ground = i;
		}
		if (tm.layers[i].name == "prefab_overlay_1") {
			prefab_layer_overlay = i;
		}
		if (tm.layers[i].name == "prefab_blocking_2") {
			prefab_blocking_2 = i;
		}
	}
	if (prefab_layer_ground == -1) {
		append_layer(tm, "prefab_ground_1");
	}
	if (blockingLayer == -1) {
		blockingLayer = append_layer(tm, "blocking");
	}
	if (prefab_blocking_2 == -1) {
		append_layer(tm, "prefab_blocking_2");
	}
	if (blockingLayer_overlay_1 == -1) {
		blockingLayer_overlay_1 = append_layer(tm, "blocking_overlay_1");
	}
	if (prefab_layer_overlay == -1) {
		append_layer(tm, "prefab_overlay_1");
	}
	bubble_layer_after(tm, "blocking", "prefab_ground_1");
	bubble_layer_after(tm, "prefab_blocking_2", "blocking");
	bubble_layer_after(tm, "blocking_overlay_1", "prefab_blocking_2");
	bubble_layer_after(tm, "prefab_overlay_1", "blocking_overlay_1");
	bubble_layer_after(tm, "overlay_1", "prefab_overlay_1");
	// Recheck blocking and blocking_overlay_1 layers as they might have been shifted while sorting.
	for (size_t i=0; i < tm.layers.size(); ++i) {
		if (tm.layers[i].name == "blocking") {
			blockingLayer = i;
		}
		if (tm.layers[i].name == "blocking_overlay_1") {
			blockingLayer_overlay_1 = i;
		}
	}
}

void World::init(std::shared_ptr<b2World>& world, const std::string& mapFileName) {
	this->physicsWorld = world;
	std::string tmx_file = sago::GetFileContent(mapFileName);
	tm = sago::tiled::string2tilemap(tmx_file);
	// We run though all the externally referenced tilesets to inject them into the tilemap
	for (size_t i = 0; i < tm.tileset.size(); ++i) {
		if (tm.tileset[i].source.length()) {
			std::string tsx_file = sago::GetFileContent("maps/"+tm.tileset[i].source);
			ts.push_back(sago::tiled::string2tileset(tsx_file));
			tm.tileset[i].alternativeSource = &ts.back();
		}
	}
	init_tilemap(tm, blockingLayer, blockingLayer_overlay_1);
	init_physics(world);
	protected_tiles.resize(tm.height*tm.width);
	for (int x=0; x < tm.width; ++x) {
		for (int y=0; y < tm.height; ++y) {
			if (x < 2 && y > tm.layers.at(0).height/2-6 && y < tm.layers.at(0).height/2+5) {
				protected_tiles[x+y*tm.width] = true;
			}
			if (x > tm.layers.at(0).width-3 && y > tm.layers.at(0).height/2-6 && y < tm.layers.at(0).height/2+5) {
				protected_tiles[x+y*tm.width] = true;
			}
			if (y < 2 && x > tm.layers.at(0).width/2-6 && x < tm.layers.at(0).width/2+5) {
				protected_tiles[x+y*tm.width] = true;
			}
			if (y > tm.layers.at(0).height-3 && x > tm.layers.at(0).width/2-6 && x < tm.layers.at(0).width/2+5) {
				protected_tiles[x+y*tm.width] = true;
			}
		}
	}
	object_group_add_group_if_not_exist(tm, "prefab_marking");
	const std::vector<sago::tiled::TileObjectGroup>& object_groups = tm.object_groups;
	for (const auto& group : object_groups) {
		for (const auto& item : group.objects) {
			if (item.isEllipse) {
				continue;
			}
			if (item.x > 0 && item.y > 0 && item.width > 0 && item.height > 0) {
				for (int x = item.x/32; x <item.x/32+(item.width+31)/32 && x < tm.width; ++x) {
					for (int y = item.y/32; y < item.y/32+(item.height+31)/32 && y < tm.height; ++y) {
						protected_tiles[x+y*tm.width] = true;
					}
				}
			}
		}
	}
}

void World::init(std::shared_ptr<b2World>& world) {
	init(world, "maps/sample1.tmx");
}

bool World::tile_protected(int x, int y) const {
	size_t index = x+y*tm.width;
	if (x<0 || y<0 || x>=tm.width || y>=tm.height || index >= protected_tiles.size()) {
		//Outside the area. Assume protected
		return true;
	}
	return protected_tiles.at(index);
}

bool World::tile_blocking(int x, int y) const {
	size_t index = x+y*tm.width;
	return blocking_tiles.at(index);
}