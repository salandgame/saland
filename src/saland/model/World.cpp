#include "World.hpp"
#include "placeables.hpp"

World::World() {
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
				b2BodyDef myBodyDef;
				myBodyDef.type = b2_staticBody;
				myBodyDef.position.Set(item.x/pixel2unit+item.width/pixel2unit/2.0, item.y/pixel2unit + item.height/pixel2unit/2.0 );
				myBodyDef.linearDamping = 1.0f;
				b2Body* body = physicsWorld->CreateBody(&myBodyDef);
				b2PolygonShape polygonShape;
				polygonShape.SetAsBox(item.width/pixel2unit/2.0, item.height/pixel2unit/2.0);
				b2FixtureDef myFixtureDef;
				myFixtureDef.shape = &polygonShape; 
				body->CreateFixture(&myFixtureDef);
			}
		}
	}
}

