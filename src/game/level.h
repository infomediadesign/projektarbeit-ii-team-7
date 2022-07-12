#ifndef __GAME_LEVEL_H
#define __GAME_LEVEL_H

#include "entities/entity.h"
#include "entities/entity_manager.h"

#include <engine/input/input.h>
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <lua.hpp>
#include <memory>
#include <simdjson.h>
#include <string>
#include <vector>

struct Background {
	std::vector<u32> background_data;
	f32 x;
	f32 y;
};

struct Object {
	std::string script;
	f32 x;
	f32 y;
	u32 width;
	u32 height;
	u32 tileset_id;
};

struct Collision {
	std::vector<u32> collisions_data;
	f32 x;
	f32 y;
};

class Level {
private:
	lua_State* lua;
	EntityManager* ent_manager;
	simdjson::ondemand::parser json_parser;
	simdjson::ondemand::document json_data, id_json_data;

	std::vector<Background> background;
	std::vector<Object> objects;
	std::vector<Collision> collisions;
	std::vector<std::string> tileset_id_json;
	std::vector<std::string> tileset_id_list;

public:
	Level(lua_State* lua) { this->lua = lua; }

	std::vector<std::shared_ptr<Entity>>* get_entities();

	std::vector<struct Background> get_background() {
		return background;
	};
	std::vector<struct Object> get_objects() {
		return objects;
	};
	std::vector<struct Collision> get_collisions() {
		return collisions;
	};
	std::vector<std::string> get_tileset_id_list() {
		return tileset_id_list;
	};

	void load_json(std::vector<std::shared_ptr<Entity>>* entities, const std::string path);
	void init();
	void update();
};

#endif