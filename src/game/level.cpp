#include "level.h"

#include <engine/input/asset.h>

void Level::load_json(const std::string path) {
  char real_path[256];

  asset_find(path.c_str(), real_path);

  simdjson::padded_string json = simdjson::padded_string::load(real_path);

  this->json_data = this->json_parser.iterate(json);
}
