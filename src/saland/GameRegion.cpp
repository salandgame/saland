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

GameRegion::GameRegion() {
	Init(0, 0, "world1");
}

static std::string createFileName(int x, int y,const std::string& worldName) {
	std::string ret = std::string("worlds/")+worldName+"/"+std::string("maps/m")+std::to_string(x)+"x"+std::to_string(y)+".tmx" ;
	return ret;
}

void GameRegion::Init(int x, int y, const std::string& worldName) {
	region_x = x;
	region_y = y;
	mapFileName = createFileName(region_x, region_y, worldName);
	std::string loadMap = mapFileName;
	if (!sago::FileExists(loadMap.c_str())) {
		loadMap = "maps/sample1.tmx";
	}
	b2Vec2 gravity(0.0f, 0.0f);
	placeables.clear();
	physicsBox.reset(new b2World(gravity));
	world.managed_bodies.clear();
	world.init(physicsBox, loadMap);
	
	std::shared_ptr<MiscItem> barrel = std::make_shared<MiscItem>();
	barrel.get()->Radius = 16.0f;
	barrel.get()->sprite = "item_barrel";
	barrel.get()->X = 100.0f;
	barrel.get()->Y = 100.0f;
	placeables.push_back(barrel);


	std::shared_ptr<Monster> bat = std::make_shared<Monster>();
	bat.get()->Radius = 16.0f;
	bat.get()->race = "bat";
	bat.get()->X = 200.0f;
	bat.get()->Y = 200.0f;
	placeables.push_back(bat);

	b2CircleShape circleShape;
	circleShape.m_p.Set(0, 0); //position, relative to body position
	circleShape.m_radius = 0.5f; //radius 16 pixel (32 pixel = 1)
	b2FixtureDef myFixtureDef;
	myFixtureDef.shape = &circleShape; //this is a pointer to the shape above
	myFixtureDef.density = 10.0f;

	b2BodyDef batBodyDef;
	batBodyDef.type = b2_dynamicBody;
	batBodyDef.position.Set(0, 0);
	batBodyDef.linearDamping = 1.0f;
	bat->body = physicsBox->CreateBody(&batBodyDef);
	b2FixtureDef batDef;
	batDef.shape = &circleShape;
	batDef.density = 10.0f;
	bat->body->CreateFixture(&batDef);
	bat->body->SetTransform(b2Vec2(bat.get()->X / 32.0f, bat.get()->Y / 32.0f),bat->body->GetAngle());

	b2BodyDef barrelBodyDef;
	barrelBodyDef.type = b2_staticBody;
	barrelBodyDef.position.Set(barrel.get()->X / 32.0f, barrel.get()->Y / 32.0f);
	barrelBodyDef.linearDamping = 1.0f;
	barrel->body = physicsBox->CreateBody(&barrelBodyDef);
	barrel->body->CreateFixture(&myFixtureDef);

}

void GameRegion::SaveRegion() {
	std::string data2save = sago::tiled::tilemap2string(world.tm);
	sago::WriteFileContent(mapFileName.c_str(), data2save);
}

