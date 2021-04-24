/*
This is a small library for reading and writing (!) Tiled map files
It is designed especially for reading the default map format produced by Tiled 0.14.2 (http://www.mapeditor.org/)

Another feature is that files can be loaded from memory. Mostly to use it with PhysFS
This does put a bit of extra pressure on the user because if the TileSet in ints own file the user must provide it.

It still has some limitations:
* Cannot auto load the tileset (cannot access the filesystem)
* Only one (1) TileSet
* No functions to actually change the tilemap

Compiles with g++ and requires C++11 support.

Requirements:
* zlib (http://www.zlib.net/)
* rapidxml (http://rapidxml.sourceforge.net/)
* b64 (http://libb64.sourceforge.net/), Although for modern compilers several patches are needed. I have used the ones from Debian

#License
Provided under the MIT license. The license is as follows:
Copyright (c) 2017 Poul Sander
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#ifndef TMX_STRUCT_H
#define TMX_STRUCT_H

#include "rapidxml/rapidxml.hpp"

#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <zlib.h>
#include <iostream>
#include "b64/decode.h"
#include "b64/encode.h"

namespace sago {
namespace tiled {

class SagoTiledException : public std::exception {
	char errMsg[256];
public:
	SagoTiledException() {
		errMsg[0] = '\0';
	}
	
	SagoTiledException(const char* fmt, ...) __attribute__ ((format (printf, 2, 3))) {
		va_list args;
		va_start(args, fmt);
		vsnprintf(errMsg, sizeof(errMsg), fmt, args);
		va_end(args);
	}
	
	virtual const char* what() const noexcept override {
		return errMsg;
	}

};

inline void* z_alloc(void *opaque __attribute__((unused)), unsigned int items, unsigned int size) {
	return realloc(NULL, items *size);
}

inline void z_free(void *opaque __attribute__((unused)), void *address) {
	free(address);
}

inline std::string zlib_decompress(const char *source, unsigned int slength) {
	std::string res;
	char buffer[4096];
	z_stream strm;
	
	strm.zalloc = z_alloc;
	strm.zfree = z_free;
	strm.opaque = Z_NULL;
	strm.next_in = (Bytef*)source;
	strm.avail_in = slength;
	strm.next_out = (Bytef*)buffer;
	strm.avail_out = sizeof(buffer);
	
	int ret=inflateInit2(&strm, 15 + 32);
	if (ret != Z_OK) {
		throw SagoTiledException("zlib error: inflateInit2 returned %d", ret);
	}
	
	while (ret == Z_OK) {
		ret = inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_OK || ret == Z_STREAM_END) {
			res.append(buffer, sizeof(buffer));
			strm.next_out = (Bytef*)buffer;
			strm.avail_out = sizeof(buffer);
		}
	}
	
	if ( ret != Z_STREAM_END) {
		inflateEnd(&strm);
		throw SagoTiledException("zlib error: inflate returned %d", ret);
	}
	res.resize(strm.total_out);
	
	inflateEnd(&strm);
	
	return res;
}

inline std::string zlib_compress(const std::string& source) {
	std::string res;
	char buffer[1024];
	z_stream strm;
	
	strm.zalloc = z_alloc;
	strm.zfree = z_free;
	strm.opaque = Z_NULL;
	strm.next_in = (Bytef*)source.data();
	strm.avail_in = source.size();
	strm.next_out = (Bytef*)buffer;
	strm.avail_out = sizeof(buffer);
	
	int ret = deflateInit2(&strm, Z_BEST_SPEED,Z_DEFLATED, 15 | 16,
              8,
              Z_DEFAULT_STRATEGY);
	if (ret != Z_OK) {
		throw SagoTiledException("zlib error: deflateInit2 returned %d", ret);
	}
	while (ret == Z_OK) {
		ret = deflate(&strm, Z_FINISH);
		if (ret == Z_OK || ret == Z_STREAM_END) {
			res.append(buffer, sizeof(buffer));
			strm.next_out = (Bytef*)buffer;
			strm.avail_out = sizeof(buffer);
		}
	}
	if ( ret != Z_STREAM_END) {
		deflateEnd(&strm);
		throw SagoTiledException("zlib error: deflate returned %d", ret);
	}
	res.resize(strm.total_out);
	
	deflateEnd(&strm);
	return res;
}

const char* const b64enc = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * This functions will skip whitespace until it finds a base64 encoding charecter. It will then decode it and decompress it.
 * @param data base64 encoded data of a zlib compressed array of gids_count uint32 elements.
 * @param gids_count Number of elements in the result.
 * @return The raw result. will be 4*gids_count large.
 */
inline std::string string_decompress_decode(const std::string &data)
{
	std::size_t found = data.find_first_of(b64enc);
	if (found == std::string::npos) {
		std::cerr << "Warning: Layer without data... this is most likely a mistake.\n";
		return "";
	}
	std::string compressed_str;
	compressed_str.resize(data.length());
	base64::decoder decoder;
	decoder.decode(&data[found], data.length()-found, &compressed_str[0]);
	return zlib_decompress(compressed_str.c_str(), compressed_str.length());
}

inline std::string string_encode( const std::string& data) {
	std::stringstream ret;
	std::stringstream input(data);
	base64::encoder E;
	E.encode(input, ret);
	return ret.str();
}

struct Terrain {
	std::string name;
	int tile;
};

struct TerrainTypes {
	std::vector<Terrain> terrains;
};

struct Tile {
	int id = 0;
	std::string terrain;
	std::string probability;
};

struct Image {
	std::string source;
	int width = 0;
	int height = 0;
};


struct TileSet {
	int firstgid = 0;
	std::string source;
	TileSet* alternativeSource = nullptr;  //< Set to non-null to use an alternative TileSet. Must be set if source is set.
	std::string name;
	int tilewidth = 0;
	int tileheight = 0;
	int spacing = 0;
	int margin = 0;
	int tilecount = 0;
	int columns = 0;
	Image image;
	std::vector<Terrain> terrainTypes;
	std::vector<Tile> tiles;
	std::map<int, Tile> tiles_map;
};

struct TileLayerData {
	std::string encoding = "base64";
	std::string compression = "zlib";
	std::string payload;
};

struct TileLayer {
	std::string name;
	int width=0;
	int height=0;
	TileLayerData data;
};

struct TileProperty {
	std::string name;
	std::string type;
	std::string value;
};

struct TileObject {
	int id = 0;
	std::string name;
	std::string type;
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	bool isEllipse = false;
	bool isPoint = false;
	std::vector<std::pair<int,int> > polygon_points;
	std::map<std::string, TileProperty> properties;
};

struct TileObjectGroup {
	std::string name;
	std::vector<TileObject> objects;
	double opacity = 1.0;
};

struct TileMap {
	std::string version;
	std::string orientation;
	std::string renderorder;
	int width = 0;
	int height = 0;
	int tilewidth=0;
	int tileheight=0;
	int nextobjectid = 0;
	std::vector<TileSet> tileset;
	std::vector<TileLayer> layers;
	std::vector<TileObjectGroup> object_groups;
};

inline void setValueFromAttribute(rapidxml::xml_node<> * node, const char* name, std::string& result) {
	auto val = node->first_attribute(name);
	if (val) {
		result = val->value();
	}
}

inline void setValueFromAttribute(rapidxml::xml_node<> * node, const char* name, int& result) {
	auto val = node->first_attribute(name);
	if (val) {
		result = std::stoi(val->value());
	}
}

inline void setValueFromAttribute(rapidxml::xml_node<> * node, const char* name, double& result) {
	auto val = node->first_attribute(name);
	if (val) {
		result = std::stod(val->value());
	}
}

inline rapidxml::xml_node<> * getElement(rapidxml::xml_node<> * node, const char* name) {
	auto val = node->first_node(name);
	if (!val) {
		throw SagoTiledException("Failed to find mandatory value: %s on %s", name, node->name());
	}
	return val;
}

inline rapidxml::xml_node<> * getElement(rapidxml::xml_node<> * node, const char* name, bool& found) {
	auto val = node->first_node(name);
	found  = val;
	return val;
}

inline TileSet node2tileset(rapidxml::xml_node<> * tileset_node) {
	TileSet ts;
	setValueFromAttribute( tileset_node, "name", ts.name);
	setValueFromAttribute( tileset_node, "source", ts.source);
	setValueFromAttribute( tileset_node, "tilewidth", ts.tilewidth);
	setValueFromAttribute( tileset_node, "tileheight", ts.tileheight);
	setValueFromAttribute( tileset_node, "tilecount", ts.tilecount);
	setValueFromAttribute( tileset_node, "firstgid", ts.firstgid);
	bool found = false;
	rapidxml::xml_node<> * image_node = getElement(tileset_node, "image", found);
	if (found) {
		setValueFromAttribute(image_node, "source", ts.image.source);
		setValueFromAttribute(image_node, "width", ts.image.width);
		setValueFromAttribute(image_node, "height", ts.image.height);
	}
	rapidxml::xml_node<> * terraintypes_node = tileset_node->first_node("terraintypes");
	if (terraintypes_node) {
		for (rapidxml::xml_node<> * terrain_node = terraintypes_node->first_node("terrain"); terrain_node; terrain_node = terrain_node->next_sibling()) {
			Terrain t;
			setValueFromAttribute(terrain_node, "name", t.name);
			setValueFromAttribute(terrain_node, "tile", t.tile);
			ts.terrainTypes.push_back(t);
		}
	}
	for (rapidxml::xml_node<> * tile_node = tileset_node->first_node("tile"); tile_node; tile_node = tile_node->next_sibling()) {
		Tile t;
		setValueFromAttribute(tile_node, "id", t.id);
		setValueFromAttribute(tile_node, "terrain", t.terrain);
		setValueFromAttribute(tile_node, "probability", t.probability);
		ts.tiles_map[t.id] = t;
		ts.tiles.push_back(t);
	}
	return ts;
}

inline TileSet string2tileset(const std::string& tsx_content) {
	std::string tsx_parseable_content = tsx_content;
	rapidxml::xml_document<> doc;    // character type defaults to char
	char* parsable_pointer = &tsx_parseable_content[0];  //Legal from C++11 and forward
	doc.parse<0>(parsable_pointer);
	rapidxml::xml_node<> * root_node = doc.first_node("tileset");
	return node2tileset(root_node);
}

inline TileMap string2tilemap(const std::string& tmx_content) {
	TileMap m;
	std::string tmx_parseable_content = tmx_content;
	rapidxml::xml_document<> doc;    // character type defaults to char
	char* parsable_pointer = &tmx_parseable_content[0];  //Legal from C++11 and forward
	doc.parse<0>(parsable_pointer);
	rapidxml::xml_node<> * root_node = doc.first_node("map");
	setValueFromAttribute( root_node, "verion", m.version);
	setValueFromAttribute( root_node, "orientation", m.orientation);
	setValueFromAttribute( root_node, "renderorder", m.renderorder);
	setValueFromAttribute( root_node, "width", m.width);
	setValueFromAttribute( root_node, "height", m.height);
	setValueFromAttribute( root_node, "tilewidth", m.tilewidth);
	setValueFromAttribute( root_node, "tileheight", m.tileheight);
	setValueFromAttribute( root_node, "nextobjectid", m.nextobjectid);
	//const auto& tileset_node = getElement(root_node, "tileset");
	for (rapidxml::xml_node<> * tileset_node = root_node->first_node("tileset"); tileset_node; tileset_node = tileset_node->next_sibling("tileset")) {
		m.tileset.push_back(node2tileset(tileset_node));
	}
	for (rapidxml::xml_node<> * layer_node = root_node->first_node("layer"); layer_node; layer_node = layer_node->next_sibling("layer")) {
		TileLayer tl;
		setValueFromAttribute(layer_node, "name", tl.name);
		setValueFromAttribute(layer_node, "height", tl.height);
		setValueFromAttribute(layer_node, "width", tl.width);
		const auto& data_node = getElement(layer_node, "data");
		setValueFromAttribute(data_node, "encoding", tl.data.encoding);
		setValueFromAttribute(data_node, "compression", tl.data.compression);
		std::string compressed_payload = data_node->value();
		tl.data.payload = sago::tiled::string_decompress_decode(compressed_payload);
		m.layers.push_back(tl);
	}
	for (rapidxml::xml_node<> * object_group_node = root_node->first_node("objectgroup"); object_group_node; object_group_node = object_group_node->next_sibling("objectgroup") ) {
		TileObjectGroup group;
		setValueFromAttribute(object_group_node, "name", group.name);
		setValueFromAttribute(object_group_node, "opacity", group.opacity);
		for (rapidxml::xml_node<> * object_node = object_group_node->first_node("object"); object_node; object_node = object_node->next_sibling("object") ) {
			TileObject to;
			setValueFromAttribute(object_node, "id", to.id);
			setValueFromAttribute(object_node, "name", to.name);
			setValueFromAttribute(object_node, "type", to.type);
			setValueFromAttribute(object_node, "x", to.x);
			setValueFromAttribute(object_node, "y", to.y);
			setValueFromAttribute(object_node, "width", to.width);
			setValueFromAttribute(object_node, "height", to.height);
			getElement(object_node, "ellipse", to.isEllipse);
			getElement(object_node, "point", to.isPoint);
			bool isPolygon = false;
			const auto& polygon = getElement(object_node, "polygon", isPolygon);
			if (isPolygon) {
				std::string points;
				setValueFromAttribute(polygon, "points", points);
				size_t points_pos = 0;
				size_t end_pos = 0;
				while( points_pos < points.size() )  {
					end_pos = points.find(",", points_pos);
					int x = std::stoi(points.substr(points_pos, end_pos));
					points_pos = end_pos+1;
					end_pos = points.find(" ", points_pos);
					if (end_pos == std::string::npos) {
						end_pos = points.size();
					}
					int y = std::stoi(points.substr(points_pos, end_pos));
					points_pos = end_pos+1;
					to.polygon_points.push_back(std::make_pair(x, y));
				}
			}
			rapidxml::xml_node<> * properties_node = object_node->first_node("properties");
			if (properties_node) {
				for (rapidxml::xml_node<> * property_node = properties_node->first_node("property"); 
				property_node; property_node = property_node->next_sibling("property") ) {
					TileProperty tp;
					setValueFromAttribute(property_node, "name", tp.name);
					setValueFromAttribute(property_node, "type", tp.type);
					setValueFromAttribute(property_node, "value", tp.value);
					to.properties[tp.name] = tp;
				}
			}
			group.objects.push_back(to);
		}
		m.object_groups.push_back(group);
	}
	return m;
}

inline void xml_add_attribute(std::iostream& io, const std::string& name, const std::string& value) {
	if (value.length()) {
		io << " " << name << "=\"" << value << "\"";
	}
}

inline void xml_add_attribute(std::iostream& io, const char* name, int value) {
	if (value) {
		io << " " << name << "=\"" << value << "\"";
	}
}

inline void xml_add_tileset(std::iostream& io, const TileMap& m, size_t tile_set_number) {
	const auto& ts = m.tileset.at(tile_set_number);
	io << "<tileset";
	xml_add_attribute(io, "firstgid", ts.firstgid);
	if (ts.source.length() > 0) {
		xml_add_attribute(io, "source", ts.source);
	}
	else {
		io << " name=\"" << ts.name << "\" tilewidth=\""<<ts.tilewidth <<"\" tileheight=\""<< ts.tileheight<< "\" tilecount=\""<<ts.tilecount<<"\"";
	}
	io << ">\n";
	if (ts.image.source.length() > 0) {
		io << "<image source=\"" << ts.image.source << "\" width=\"" << ts.image.width << "\" height=\"" << ts.image.height << "\"/>\n";
	}
	io << "</tileset>\n";
}

inline void xml_add_layer(std::iostream& io, const TileMap& m, size_t layer_number) {
	const TileLayer& l = m.layers.at(layer_number);
	io << "<layer";
	xml_add_attribute(io, "name", l.name);
	xml_add_attribute(io, "height", l.height);
	xml_add_attribute(io, "width", l.width);
	io << ">\n";
	io << "<data encoding=\"base64\" compression=\"zlib\">\n";
	io << string_encode(zlib_compress(l.data.payload));
	io << "</data>\n";
	io << "</layer>\n";
}

inline void xml_add_objectgroup(std::iostream& io, const TileMap& m, size_t object_group_number) {
	const TileObjectGroup& tog = m.object_groups.at(object_group_number);
	io << "<objectgroup name=\"" << tog.name << "\" opacity=\"" << tog.opacity << "\"";
	io << ">\n";
	for (const TileObject& to : tog.objects) {
		io << "<object";
		io << " id=\"" << to.id << "\"";
		if (to.name.length() > 0) {
			io << " name=\"" << to.name << "\"";
		}
		if (to.type.length() > 0) {
			io << " type=\"" << to.type << "\"";
		}
		io << " x=\"" << to.x << "\" y=\"" << to.y << "\" width=\"" << to.width << "\" height=\"" << to.height << "\"";
		io << ">\n";
		if (to.isEllipse) {
			io << "<ellipse/>\n";
		}
		if (to.isPoint) {
			io << "<point/>\n";
		}
		if (to.polygon_points.size() > 0) {
			io << "<polygon points=\"";
			for (size_t i=0; i < to.polygon_points.size(); ++i) {
				if (i!=0) {
					io << " ";
				}
				io << to.polygon_points.at(i).first << "," << to.polygon_points.at(i).second;
			}
			io << "\"/>\n";
		}
		if (to.properties.size() > 0) {
			io << "<properties>\n";
			for (const auto& prop : to.properties) {
				io << "<property name=\"" << prop.first << "\" ";
				if (prop.second.type.size()> 0) {
					io << "type=\"" << prop.second.type << "\" ";
				}
				io << "value=\"" << prop.second.value << "\" ";
				io << "/>\n";
			}
			io << "</properties>\n";
		}
		io << "</object>\n";
	}
	io << "</objectgroup>\n";
}

inline std::string tilemap2string(const TileMap& m) {
	//Assuming UTF-8 because rapidxml ignores it.
	std::stringstream ret;
	ret << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ret << "<map version=\"1.0\"";
	xml_add_attribute(ret,  "orientation", m.orientation);
	xml_add_attribute(ret,  "renderorder", m.renderorder);
	xml_add_attribute(ret,  "width", m.width);
	xml_add_attribute(ret,  "height", m.height);
	xml_add_attribute(ret,  "tilewidth", m.tilewidth);
	xml_add_attribute(ret,  "tileheight", m.tileheight);
	xml_add_attribute(ret,  "nextobjectid", m.nextobjectid);
	ret << ">\n";
	for (size_t i = 0; i < m.tileset.size(); ++i) {
		xml_add_tileset(ret, m, i);
	}
	for (size_t i = 0; i < m.layers.size(); ++i) {
		xml_add_layer(ret, m, i);
	}
	for (size_t i = 0; i < m.object_groups.size(); ++i) {
		xml_add_objectgroup(ret, m, i);
	}
	ret << "</map>\n";
	return ret.str();
}

/**
 * This function returns the coordinates from a texture to find the gid.
 * Note that this might not work correctly at the moment and will fail in case of more than one tileset
 * @param tm The map to take the tileset from,
 * @param gid The gid to use. Note that this must be 1 or larger. An invalid gid yilds undefined behaivior.
 * @param[out] imageFile The image filename will be written to this pointer if not NULL. May be null.
 * @param[out] x The X coordinate will be written to this pointer if not NULL. May be null.
 * @param[out] y The y coordinate will be written to this pointer if not NULL. May be null.
 * @param[out] w The tile width will be written to this pointer if not NULL. May be null.
 * @param[out] h The tile height will be written to this pointer if not NULL. May be null.
 */
inline void getTextureLocationFromGid(const TileMap& tm, int gid, std::string* imageFile, int* x, int* y, int* w, int* h ) {
	//Currently hardcodeed to one tileset
	const TileSet *ts = nullptr;
	if (tm.tileset.size() < 1) {
		return;
	}
	ts = &(tm.tileset.at(0));
	for (size_t i = 1; i < tm.tileset.size(); ++i) {
		const TileSet *newTs = &(tm.tileset.at(i));
		if (newTs->firstgid > gid) {
			break;
		}
		ts = newTs;
	}
	gid -= ts->firstgid;  //Final gid needs to be set before we check for alternative source
	while (ts->alternativeSource) {
		//Follow source links
		ts = ts->alternativeSource;
	}
	//std::cout << "ts: " << ts->name << ", " << ts->image.source << ", " << ts->image.height << ", " << ts->image.width << ", gid: " << gid+ts->firstgid << "\n";
	if (imageFile) {
		*imageFile = ts->image.source;
	}
	if (ts->image.width == 0) {
		std::cerr << "image: " << ts->image.source << " (" << ts->image.width << ", " << ts->image.height << ") is not good\n";
		abort();
	}
	if (x) {
		*x = ( gid *ts->tilewidth)%ts->image.width;
	}
	if (y) {
		*y = ( gid *ts->tilewidth)/ts->image.width* ts->tilewidth;
	}
	if (w) {
		*w = ts->tilewidth;
	}
	if (h) {
		*h = ts->tileheight;
	}
}

inline bool tileInBound(const TileMap& tm, int x, int y) {
	if (x < 0 || y < 0 || x >= tm.width || y >= tm.height) {
		return false;
	}
	return true;
}

/**
 * This function tells the gid of the tile in a given location on a given map, on a given layer
 *
 * If the layer is corrupt or the coordinates out of bound then the behavior is unspecified.
 * It may return the wrong gid or throw a SagoTiledException
 *
 * @param m The TileMap to look at. Required because the total size are located here
 * @param l The layer to look at
 * @param x The X coordinate
 * @param y The Y coordinate
 * @return The gid number of the given tile. Provided that X and Y are within the map limits
 */
inline uint32_t getTileFromLayer(const TileMap& m, const TileLayer& l, int x, int y) {
	size_t tile_index = (m.height*y+x)*sizeof(uint32_t);
	if (tile_index > l.data.payload.size()-sizeof(uint32_t) ) {
		return 0;
		/*throw SagoTiledException("ERROR: getTileFromLayer called with coordinates out-of-bound. Called with (%d, %d). Limit (%d, %d). Or the layer is corrupt. "
				"Reported number of tiles in layer: %ld", x, y, m.width-1, m.height-1, (long int)l.data.payload.size()/sizeof(uint32_t));*/
	}
	const unsigned char *data = reinterpret_cast<const unsigned char*>(l.data.payload.data());
	uint32_t global_tile_id = data[tile_index] |
				data[tile_index + 1] << 8 |
				data[tile_index + 2] << 16 |
				data[tile_index + 3] << 24;
	return global_tile_id;
}

inline TileLayer createEmptyLayerForMap(const TileMap& tm) {
	TileLayer t;
	t.width = tm.width;
	t.height = tm.height;
	t.data.payload.resize(t.width*t.height*sizeof(uint32_t));
	std::string& data = t.data.payload;
	memset(&data[0], 0, data.size());
	return t;
}

inline void setTileOnLayerNumber(TileMap& m, int layer_number, int x, int y, uint32_t tile) {
	size_t tile_index = (m.height*y+x)*sizeof(uint32_t);
	TileLayer& l = m.layers.at(layer_number);
	if (tile_index > l.data.payload.size()-sizeof(uint32_t) ) {
		throw SagoTiledException("ERROR: setTileOnLayerNumber called with coordinates out-of-bound. Called with (%d, %d). Limit (%d, %d). Or the layer is corrupt. "
				"Reported number of tiles in layer: %ld", x, y, m.width-1, m.height-1, (long int)l.data.payload.size()/sizeof(uint32_t));
	}
	std::string& data = l.data.payload;
	uint32_t* dp = reinterpret_cast<uint32_t*> (&data[tile_index]);
	*dp = tile;
}

}  //tiled
}  //sago

#endif /* TMX_STRUCT_H */

