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
    INT64 x;
    INT64 y;
    std::vector<u32> background_data;
    Background(u32 _x, u32 _y,
        std::vector<u32>&& _background_data) :
        x{ _x }, y{ _y }, background_data(_background_data) {}
};

struct Objects {
    INT64 x;
    INT64 y;
    u32 width;
    u32 height;
    u32 tileset_id;
    std::string script;

    Objects(u32 _x, u32 _y, u32 _width, u32 _height, u32 _tile_set_id, std::string_view _script) :
        x{ _x }, y{ _y }, width{ _width }, height{ _height }, tileset_id{ _tile_set_id }, script{ _script } {}
};

struct Collisions {
    INT64 x;
    INT64 y;
    std::vector<u32> collisions_data;
    Collisions(u32 _x, u32 _y,
        std::vector<u32>&& _background_data) :
        x{ _x }, y{ _y }, collisions_data(_background_data) {}
};

class Level {
private:
  lua_State *lua;
  EntityManager* ent_manager;
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

  std::vector<struct Background> get_background() {
      return background_content;
  };
  std::vector<struct Objects> get_objects() {
      return objects_content;
  };
  std::vector<struct Collisions> get_collisions() {
      return collisions_content;
  };
  std::vector<std::string> get_tileset_id_list() {
      return tileset_id_list;
  };

  void load_json(std::vector<std::shared_ptr<Entity>> *entities, const std::string path);
  void update();
};

#endif
