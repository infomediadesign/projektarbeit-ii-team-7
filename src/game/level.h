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
  i64 x;
  i64 y;
  std::vector<u32> background_data;
};

struct Objects {
  i64 x;
  i64 y;
  u32 width;
  u32 height;
  u32 tileset_id;
  std::string script;
};

struct Collisions {
  i64 x;
  i64 y;
  std::vector<u64> collisions_data;
};

class Level {
private:
  lua_State *lua;
  EntityManager *ent_manager;
  simdjson::ondemand::parser json_parser;
  simdjson::ondemand::document json_data, id_json_data;

  std::vector<Background> background_content;
  std::vector<Objects> objects_content;
  std::vector<Collisions> collisions_content;
  std::vector<std::string> tileset_id_jsons;
  std::vector<std::string> tileset_id_list;

public:
  Level(lua_State *lua) { this->lua = lua; }

  std::vector<std::shared_ptr<Entity>> *get_entities();

  std::vector<Background> get_background() { return background_content; };

  std::vector<Objects> get_objects() { return objects_content; };

  std::vector<Collisions> get_collisions() { return collisions_content; };

  std::vector<std::string> get_tileset_id_list() { return tileset_id_list; };

  void load_json(std::vector<std::shared_ptr<Entity>> *entities, const std::string path);
  void update();
};

#endif
