#include "World.hpp"
#include "placeables.hpp"

World::World() {
}

static void AddStaticTilesToWorld(b2World* world, const sago::tiled::TileMap& tm, const sago::tiled::TileLayer& layer) {
	for (int i = 0; i < tm.height; ++i) {
		for (int j = 0; j < tm.width; ++j) {
			uint32_t gid = sago::tiled::getTileFromLayer(tm, layer, i, j);
			if (gid == 0) {
				continue;
			}
			b2BodyDef myBodyDef;
				myBodyDef.type = b2_staticBody;
				myBodyDef.position.Set(i*32.0f/pixel2unit+32.0f/pixel2unit/2.0, j*32.0f/pixel2unit + 32.0f/pixel2unit/2.0 );
				myBodyDef.linearDamping = 1.0f;
				b2Body* body = world->CreateBody(&myBodyDef);
				b2PolygonShape polygonShape;
				polygonShape.SetAsBox(32.0f/pixel2unit/2.0, 32.0f/pixel2unit/2.0);
				b2FixtureDef myFixtureDef;
				myFixtureDef.shape = &polygonShape; 
				body->CreateFixture(&myFixtureDef);
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
	const std::vector<sago::tiled::TileLayer>& layers = tm.layers;
	for (const auto& layer : layers) {
		if (layer.name != "blocking") {
			continue;
		}
		AddStaticTilesToWorld(physicsWorld.get(), tm, layer);
	}
}

