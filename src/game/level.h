#ifndef __GAME_LEVEL_H
#define __GAME_LEVEL_H

#include "entities/entity.h"

#include <engine/input/input.h>
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <memory>
#include <simdjson.h>
#include <string>
#include <vector>
#include <lua.hpp>

class Level {
private:
  lua_State *lua;
  simdjson::ondemand::parser json_parser;
  simdjson::ondemand::document json_data;

public:
  Level(lua_State *lua) {
    this->lua = lua;
  }
  
  std::vector<std::shared_ptr<Entity>> *get_entities();

  void load_json(std::vector<std::shared_ptr<Entity>> *entities, const std::string path);
  void update();
};

#endif
