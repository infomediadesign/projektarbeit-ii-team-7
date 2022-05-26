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

class Level {
private:
  std::vector<std::shared_ptr<Entity>> entities;
  simdjson::ondemand::parser json_parser;
  simdjson::ondemand::document json_data;

public:
  Level() {}

  void load_json(const std::string path);
};

#endif
