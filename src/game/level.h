#ifndef __GAME_LEVEL_H
#define __GAME_LEVEL_H

#include "entities/entity.h"
#include "entities/entity_manager.h"

#include <engine/types/numeric.h>
#include <lua.hpp>
#include <simdjson.h>
#include <string>
#include <vector>

struct LevelBackgroundTile {
  i64 tileset_id;
  f32 x;
  f32 y;
};

struct LevelObject {
  std::string script;
  i64 width;
  i64 height;
  i64 tileset_id;
  f32 x;
  f32 y;
};

struct LevelCollisionTile {
  f32 x;
  f32 y;
};

class Level {
private:
  std::vector<LevelBackgroundTile> background;
  std::vector<LevelObject> objects;
  std::vector<LevelCollisionTile> collisions;
  std::vector<std::string> tilesets;

  lua_State *lua;
  EntityManager *ent_manager;
  simdjson::ondemand::parser json_parser;
  simdjson::ondemand::document json_data;
  bool loaded;

  void parse_layers(simdjson::ondemand::array layers);
  void parse_background_layer(simdjson::ondemand::value layer);
  void parse_collision_layer(simdjson::ondemand::value layer);
  void parse_object_layer(simdjson::ondemand::value layer);
  void parse_tilesets(simdjson::ondemand::array tilesets);
  void load_tileset(const std::string_view path);

public:
  Level(lua_State *lua, EntityManager *ent_manager) {
    this->lua         = lua;
    this->ent_manager = ent_manager;
    this->loaded      = false;
  }

  void load_json(const std::string path);
  void init();
  void update();
};

#endif
