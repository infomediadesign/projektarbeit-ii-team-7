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

struct BackgroundJSON {
  std::vector<i64> background_data;
  f64 x;
  f64 y;

  BackgroundJSON(std::vector<i64> &&background_data, const f64 x, const f64 y) :
      background_data(std::move(background_data)), x(x), y(y) {}
};

struct ObjectJSON {
  std::string script;
  f64 x;
  f64 y;
  i64 width;
  i64 height;
  i64 tileset_id;

  ObjectJSON(
    const std::string_view script, const f64 x, const f64 y, const i64 width, const i64 height, const i64 tile_set_id
  ) :
      script(script),
      x(x), y(y), width(width), height(height), tileset_id(tileset_id) {}
};

struct CollisionJSON {
  std::vector<i64> collision_data;
  f64 x;
  f64 y;

  CollisionJSON(std::vector<i64> &&background_data, const f64 x, const f64 y) :
      collision_data(std::move(background_data)), x(x), y(y) {}
};

class Level {
private:
  std::vector<BackgroundJSON> background;
  std::vector<ObjectJSON> objects;
  std::vector<CollisionJSON> collisions;

  std::vector<std::string> tileset_id_json;
  std::vector<std::string> tileset_id_list;

  lua_State *lua;
  EntityManager *ent_manager;
  simdjson::ondemand::parser json_parser;
  simdjson::ondemand::document json_data;

public:
  Level(lua_State *lua) { this->lua = lua; }

  std::vector<struct BackgroundJSON> get_background() const { return this->background; };

  std::vector<struct ObjectJSON> get_objects() const { return this->objects; };

  std::vector<struct CollisionJSON> get_collisions() const { return this->collisions; };

  std::vector<std::string> get_tileset_id_list() const { return this->tileset_id_list; };

  void load_json(std::vector<std::shared_ptr<Entity>> *entities, const std::string path);
  void init();
  void update();
};

#endif
