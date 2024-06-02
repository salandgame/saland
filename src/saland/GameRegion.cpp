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

#include "GameRegion.hpp"
#include "GameItems.hpp"
#include <cmath>
#include <random>

GameRegion::GameRegion() {
	Init(0, 0, "world1", false);
}

static std::string createFileName(int x, int y,const std::string& worldName) {
	std::string ret = std::string("worlds/")+worldName+"/"+std::string("maps/m")+std::to_string(x)+"x"+std::to_string(y)+".tmx" ;
	return ret;
}



void GameRegion::SpawnMonster(const MonsterDef& def, float destX, float destY) {
	std::shared_ptr<Monster> bat = std::make_shared<Monster>();
	bat.get()->Radius = def.radius;
	bat.get()->race = def.race;
	bat.get()->X = destX;
	bat.get()->Y = destY;
	placeables.push_back(bat);

	b2BodyDef batBodyDef;
	batBodyDef.type = b2_dynamicBody;
	batBodyDef.position.Set(0, 0);
	batBodyDef.linearDamping = 1.0f;
	bat->body = physicsBox->CreateBody(&batBodyDef);

	b2CircleShape circleShape;
	circleShape.m_p.Set(0, 0); //position, relative to body position
	circleShape.m_radius = def.radius/32.0f; //32 pixel = 1 unit

	b2FixtureDef batDef;
	batDef.shape = &circleShape;
	batDef.density = 10.0f;
	bat->body->CreateFixture(&batDef);
	bat->body->SetTransform(b2Vec2(bat.get()->X / 32.0f, bat.get()->Y / 32.0f),bat->body->GetAngle());
}

static bool Intersect(const Placeable& p1, const Placeable& p2) {
	double distance = std::sqrt( std::pow(p1.X-p2.X, 2) + std::pow(p1.Y-p2.Y,2));
	if (distance < p1.Radius + p2.Radius) {
		return true;
	}
	return false;
}

void GameRegion::SpawnPrefab(const Prefab& prefab, int destX, int destY) {
	for (int i=destX; i < destX+prefab.width; ++i) {
		for (int j=destY; j < destY+prefab.height; ++j) {
			if (world.tile_protected(i, j)) {
				std::cout << "Do not spawn prefab " << prefab.name << "(" << i << "," << j << "), Protected tile\n";
				return;
			}
			if (world.tile_blocking(i, j)) {
				std::cout << "Do not spawn prefab " << prefab.name << "(" << i << "," << j << "), Blocked tile\n";
				return;
			}
		}
	}
	ApplyPrefab(world.tm, destX, destY, prefab);
	this->world.init_physics(this->physicsBox);
}

void GameRegion::SpawnItem(const ItemDef& def, float destX, float destY) {
	std::shared_ptr<MiscItem> barrel = std::make_shared<MiscItem>();
	barrel.get()->Radius = def.radius;
	barrel.get()->sprite = def.sprite;
	barrel.get()->sprite2 = def.sprite2;
	barrel.get()->X = destX;
	barrel.get()->Y = destY;
	barrel.get()->destructible = def.isDestructible;
	barrel.get()->health = def.health;
	barrel.get()->name = def.itemid;
	barrel.get()->pickup = def.pickup;
	for (int i=(destX-def.radius)/32; i <= (destX+def.radius)/32+1; ++i) {
		for (int j=(destY-def.radius)/32; j <= (destY+def.radius)/32+1; ++j) {
			if (world.tile_protected(i, j)) {
				std::cout << "Do not spawn " << def.itemid << "(" << destX << "," << destY << "), Protected tile\n";
				return;
			}
			if (world.tile_blocking(i, j)) {
				std::cout << "Do not spawn " << def.itemid << "(" << destX << "," << destY << "), Blocked tile\n";
				return;
			}
		}
	}

	for (std::shared_ptr<Placeable>& target : placeables) {
		if (target->removeMe) {
			continue;
		}
		if (Intersect(*barrel.get(), *target.get())) {
			std::cout << "Do not spawn " << def.itemid << "(" << destX << "," << "destY" << "), already an item placed\n";
			return;
		}
	}
	placeables.push_back(barrel);

	if (def.isStatic) {
		b2CircleShape circleShape;
		circleShape.m_p.Set(0, 0); //position, relative to body position
		circleShape.m_radius = def.radius/32.0f; //32 pixel = 1 unit
		b2FixtureDef myFixtureDef;
		myFixtureDef.shape = &circleShape; //this is a pointer to the shape above
		myFixtureDef.density = 10.0f;


		b2BodyDef barrelBodyDef;
		barrelBodyDef.type = b2_staticBody;
		barrelBodyDef.position.Set(barrel.get()->X / 32.0f, barrel.get()->Y / 32.0f);
		barrelBodyDef.linearDamping = 1.0f;
		barrel->body = physicsBox->CreateBody(&barrelBodyDef);
		barrel->body->CreateFixture(&myFixtureDef);
	}
}

static std::string GetRegionType(int region_x, int region_y) {
	if (region_x < 1) {
		return "forrest";
	}
	if (region_x == 0 && region_y == 0) {
		return "start";
	}
	return "default";
}

const int LAKE_WIDTH = 20; // width of the lake pattern
const int LAKE_HEIGHT = 20; // height of the lake pattern
const int LAKE_SIZE = 40; // maximum size of the lake
const int LAKE_MIN_SIZE = 8;

/**
 * function to generate a random lake pattern
 */
static std::vector<std::vector<int>> generateLakePattern() {
	std::vector<std::vector<int>> pattern(LAKE_WIDTH, std::vector<int>(LAKE_HEIGHT, 0));

	// choose a random point in the pattern as the center of the lake
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist_x(0, LAKE_WIDTH - 1);
	std::uniform_int_distribution<int> dist_y(0, LAKE_HEIGHT - 1);
	int center_x = LAKE_WIDTH/2;
	int center_y = LAKE_HEIGHT/2;

	// set the center point of the lake to be a lake tile
	pattern[center_x][center_y] = 1;

	// choose a random size for the lake (up to the maximum size)
	std::uniform_int_distribution<int> dist_size(LAKE_MIN_SIZE, LAKE_SIZE);
	int size = dist_size(gen);

	// randomly expand the lake outwards from the center point
	for (int i = 0; i < size; ++i) {
		// choose a random direction to expand the lake in
		std::uniform_int_distribution<int> dist_dir(0, 3);
		int dir = dist_dir(gen);

		// expand the lake in the chosen direction
		if (dir == 0) {
			if (center_x > 0) {
				--center_x;
				pattern[center_x][center_y] = 1;
			}
		}
		else if (dir == 1) {
			if (center_x < LAKE_WIDTH - 1) {
				++center_x;
				pattern[center_x][center_y] = 1;
			}
		}
		else if (dir == 2) {
			if (center_y > 0) {
				--center_y;
				pattern[center_x][center_y] = 1;
			}
		}
		else {
			if (center_y < LAKE_HEIGHT - 1) {
				++center_y;
				pattern[center_x][center_y] = 1;
			}
		}
	}
	for (size_t i = 0; i < pattern.size(); ++i) {
		int lastTile = 0;
		for (size_t j = 0; j < pattern.at(0).size(); ++j) {
			if (lastTile == 1 && pattern[i][j] == 0 ) {
				lastTile = 0;
				pattern[i][j] = 1;
			}
			else {
				lastTile = pattern[i][j];
			}
		}
	}
	for (size_t j = 0; j < pattern.at(0).size(); ++j) {
		int lastTile = 0;
		for (size_t i = 0; i < pattern.size(); ++i) {
			if (lastTile == 1 && pattern[i][j] == 0) {
				lastTile = 0;
				pattern[i][j] = 1;
			}
			else {
				lastTile = pattern[i][j];
			}
		}
	}

	return pattern;
}

void GameRegion::ProcessRegionFirstTimeEnter(World& world) {
	if (world.tm.properties["type"].value == "forrest") {
		{
			// Add a small lake
			int x = (rand()%(world.tm.width-LAKE_WIDTH));
			int y = (rand()%(world.tm.height-LAKE_HEIGHT));
			const auto& pattern = generateLakePattern();
			for (size_t i=0; i < LAKE_WIDTH; ++i) {
				for (size_t j=0; j < LAKE_HEIGHT; ++j) {
					if (pattern[i][j] == 0) {
						continue;
					}
					int layer_number = world.blockingLayer;
					uint32_t tile = 28;
					sago::tiled::setTileOnLayerNumber(world.tm, layer_number, x+i, y+j, tile);
					liqudHandler["water"].updateFirstTile(world.tm, x+i, y+j);
				}
			}
		}
	}
}

void GameRegion::ProcessRegionEnter(World& world) {
	if (world.tm.properties["type"].value == "forrest") {
		std::cout << "Forrest (or start) region\n";
		ItemDef pineDef = getItem("tree_pine");
		for (int i=0; i<10; ++i) {
			int x = (rand()%(world.tm.width-3)+1)*32+rand()%32;
			int y = (rand()%(world.tm.height-3)+1)*32+rand()%32;
			SpawnItem(pineDef, x, y);
			std::cout << "Spawning at " << x << ", " << y << "\n";
		}
	}
}


static std::string RegionChooseMapTemplate(int region_x, int region_y) {
	std::string loadMap = "maps/desert.tmx";
	if (GetRegionType(region_x, region_y) == "forrest") {
		loadMap = "maps/template_forrest.tmx";
	}
	if (GetRegionType(region_x, region_y) == "start") {
		loadMap = "maps/template_start.tmx";
	}
	if (region_x == 0 && region_y == -2) {
		loadMap = "maps/city_0x-2.tmx";
	}
	if (region_x == 3 && region_y == 3) {
		loadMap = "maps/template_forrest2.tmx";
	}
	return loadMap;
}

void GameRegion::InitCommon() {
	placeables.clear();
	physicsBox.reset(new b2World(b2Vec2(0.0f, 0.0f)));
	world.managed_bodies.clear();

	liqudHandler["water"].blockingLayer = world.blockingLayer;
	liqudHandler["water"].blockingLayer_overlay_1 = world.blockingLayer_overlay_1;
	liqudHandler["water"].setupTiles(28);
	liqudHandler["lava"].blockingLayer = world.blockingLayer;
	liqudHandler["lava"].blockingLayer_overlay_1 = world.blockingLayer_overlay_1;
	liqudHandler["lava"].setupTiles(16);

}

void GameRegion::InitDungeon(const std::string& dungeonName, const std::string& dungeonType, bool forceResetWorld) {
	InitCommon();
	world.init(physicsBox, "maps/"+dungeonName+".tmx");
}


void GameRegion::Init(int x, int y, const std::string& worldName, bool forceResetWorld) {
	bool newRegion = false;  //If this is a new region to be generated
	region_x = x;
	region_y = y;
	mapFileName = createFileName(region_x, region_y, worldName);
	std::string loadMap = mapFileName;
	if (!sago::FileExists(loadMap.c_str()) || forceResetWorld) {
		loadMap = RegionChooseMapTemplate(region_x, region_y);
		newRegion = true;
	}

	InitCommon();
	ScanPrefabs("prefabs01");
	world.init(physicsBox, loadMap);


	const std::vector<sago::tiled::TileObjectGroup>& object_groups = world.tm.object_groups;
	for (const auto& group : object_groups) {
		for (const auto& item : group.objects) {
			if (item.type == "itemSpawn") {
				std::string itemname;
				const auto& itr = item.properties.find("itemname");
				if (itr != item.properties.end()) {
					itemname = itr->second.value;
				}
				if (itemname[0]) {
					ItemDef itemDef = getItem(itemname);
					SpawnItem(itemDef, item.x, item.y);
				}
			}
		}
	}

	ItemDef barrelDef = getItem("barrel");
	SpawnItem(barrelDef, 100.0f, 100.0f);
	SpawnItem(barrelDef, 100.0f, 100.0f+32.0f);
	SpawnItem(barrelDef, 100.0f, 100.0f+64.0f);


	ItemDef pineDef = getItem("tree_pine");
	SpawnItem(pineDef, 200.0f, 400.0f);


	ItemDef potatoDef = getItem("food_potato");
	SpawnItem(potatoDef, 600.0f, 20.0f);


	//Forrest region
	std::string regionType = GetRegionType(region_x, region_y);
	if (regionType == "forrest" || regionType == "start" ) {
		world.tm.properties["type"].value = "forrest";
	}
	if (newRegion) {
		ProcessRegionFirstTimeEnter(world);
	}
	ProcessRegionEnter(world);

	SpawnPrefab(getPrefab("basic_house"), 32, 32);

	MonsterDef batDef;
	batDef.radius = 16.0f;
	batDef.race = "bat";
	SpawnMonster(batDef, 200.0f, 200.0f);
	SpawnMonster(batDef, 1200.0f, 1400.0f);
}

void GameRegion::SaveRegion() {
	sago::tiled::TileObjectGroup tog;
	tog.name = "mutableObjects";
	for (const std::shared_ptr<Placeable>& p : placeables) {
		if (p->isStatic()) {
			const MiscItem* m = dynamic_cast<MiscItem*>(p.get());
			if (m) {
				sago::tiled::TileObject to;
				to.isPoint = true;
				to.name = m->name;
				to.type = "itemSpawn";
				to.properties["itemname"].value = to.name;
				to.x = m->X;
				to.y = m->Y;
				to.id = tog.objects.size()+1000;
				tog.objects.push_back(to);
			}
		}
	}
	int mutableLayer = -1;
	for (size_t i = 0; i < world.tm.object_groups.size(); ++i) {
		const auto& l = world.tm.object_groups.at(i);
		if (l.name == tog.name) {
			mutableLayer = i;
		}
	}
	if (mutableLayer == -1) {
		world.tm.object_groups.push_back(tog);
	}
	else {
		world.tm.object_groups.at(mutableLayer) = tog;
	}
	std::string data2save = sago::tiled::tilemap2string(world.tm);
	sago::WriteFileContent(mapFileName.c_str(), data2save);
}

