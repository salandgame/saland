#include "World.hpp"

World::World() {
}

void World::init() {
	std::string tsx_file = sago::GetFileContent("maps/terrain.tsx");
	std::string tmx_file = sago::GetFileContent("maps/sample1.tmx");
	ts = sago::tiled::string2tileset(tsx_file);
	tm = sago::tiled::string2tilemap(tmx_file);
	tm.tileset.alternativeSource = &ts;
}

