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

/*
Constructors are needed, otherwise it will fail to compile.
*/

struct BackgroundJSON {
	std::vector<i64> background_data;
	f32 x;
	f32 y;
	BackgroundJSON(std::vector<i64>&& _background_data, f32 _x, f32 _y) :
		background_data(_background_data), x{ _x }, y{ _y } {}
};

struct ObjectJSON {
	std::string script;
	f32 x;
	f32 y;
	int64_t width;
	int64_t height;
	int64_t tileset_id;

	ObjectJSON(std::string_view _script, f32 _x, f32 _y, int64_t _width, int64_t _height, int64_t _tile_set_id) :
		script{ _script }, x{ _x }, y{ _y }, width{ _width }, height{ _height }, tileset_id{ _tile_set_id }  {}
};

struct CollisionJSON {
	std::vector<i64> collision_data;
	f32 x;
	f32 y;

	CollisionJSON(std::vector<i64>&& _background_data, f32 _x, f32 _y) :
		collision_data(_background_data), x{ _x }, y{ _y } {}
};

class Level {
private:
	std::vector<BackgroundJSON> background;
	std::vector<ObjectJSON> objects;
	std::vector<CollisionJSON> collisions;

	std::vector<std::string> tileset_id_json;
	std::vector<std::string> tileset_id_list;

	lua_State* lua;
	EntityManager* ent_manager;
	simdjson::ondemand::parser json_parser;
	simdjson::ondemand::document json_data, id_json_data;

public:
	Level(lua_State* lua) { this->lua = lua; }

	std::vector<std::shared_ptr<Entity>>* get_entities();

	std::vector<struct BackgroundJSON> get_background() {
		return background;
	};
	std::vector<struct ObjectJSON> get_objects() {
		return objects;
	};
	std::vector<struct CollisionJSON> get_collisions() {
		return collisions;
	};
	std::vector<std::string> get_tileset_id_list() {
		return tileset_id_list;
	};

	void load_json(std::vector<std::shared_ptr<Entity>>* entities, const std::string path);
	void level_init();
	void update();
};

#endif