#include "level.h"

#include <engine/input/asset.h>
#include <iostream>
#include <memory>

void Level::load_json(const std::string path) {
  char real_path[256];

  asset_find(path.c_str(), real_path);

  simdjson::padded_string json;

  try {
    json = simdjson::padded_string::load(real_path);
  } catch (...) {
    std::cout << "Level " << path << " does not exist!" << std::endl;
    return;
  }

  this->json_data = this->json_parser.iterate(json);

  try {
    this->parse_layers(this->json_data["layers"]);
    this->parse_tilesets(this->json_data["tilesets"]);
  } catch (...) {
    std::cout << "Level " << path << " is invalid!" << std::endl;
    return;
  }

  this->loaded = true;
}

void Level::parse_layers(simdjson::ondemand::array layers) {
  for (simdjson::ondemand::value layer : layers) {
    if (layer.find_field_unordered("type") == std::string_view("tilelayer"))
      if (layer.find_field_unordered("name") == std::string_view("collision"))
        this->parse_collision_layer(layer);
      else
        this->parse_background_layer(layer);
    else if (layer.find_field_unordered("type") == std::string_view("objectgroup"))
      this->parse_object_layer(layer);
  }
}

void Level::parse_background_layer(simdjson::ondemand::value layer) {
  for (simdjson::ondemand::value chunk : layer.find_field_unordered("chunks").get_array()) {
    const i64 height = chunk.find_field("height");
    const i64 width  = chunk.find_field("width");
    const i64 x      = chunk.find_field("x");
    const i64 y      = chunk.find_field("y");
    u32 i            = 0;

    for (simdjson::ondemand::value data : chunk.find_field_unordered("data").get_array()) {
      const i64 tileset_id = data.get_int64();

      if (tileset_id > 0) {
        this->background.push_back(
          { .tileset_id = tileset_id, .x = x * 0.1f + (i % width) * 0.1f, .y = y * 0.1f + (i / height) * 0.1f }
        );
      }

      i++;
    }
  }
}

void Level::parse_collision_layer(simdjson::ondemand::value layer) {
  for (simdjson::ondemand::value chunk : layer.find_field_unordered("chunks").get_array()) {
    const i64 height = chunk.find_field("height");
    const i64 width  = chunk.find_field("width");
    const i64 x      = chunk.find_field("x");
    const i64 y      = chunk.find_field("y");
    u32 i            = 0;

    for (simdjson::ondemand::value data : chunk.find_field_unordered("data").get_array()) {
      const i64 tileset_id = data.get_int64();

      if (tileset_id > 0)
        this->collisions.push_back({ .x = x * 0.1f + (i % width) * 0.1f, .y = y * 0.1f + (i / height) * 0.1f });

      i++;
    }
  }
}

void Level::parse_object_layer(simdjson::ondemand::value layer) { /* todo: parse objects */
}

void Level::parse_tilesets(simdjson::ondemand::array tilesets) {
  for (simdjson::ondemand::value tileset : tilesets)
    this->load_tileset(tileset.find_field_unordered("source").get_string());
}

std::string absolute_path(const std::string str) {
  if (str.rfind("../", 0) != std::string::npos)
    return absolute_path(str.substr(3, str.length()));

  return str;
}

void Level::load_tileset(const std::string_view path) {
  char real_path[256];

  asset_find(std::string(path).c_str(), real_path);

  simdjson::padded_string json;

  try {
    json = simdjson::padded_string::load(real_path);
  } catch (...) {
    std::cout << "Tileset " << path << " does not exist!" << std::endl;
    return;
  }

  try {
    simdjson::ondemand::document data = this->json_parser.iterate(json);
    std::string_view image            = data.find_field("image").get_string();
    this->tilesets.push_back(absolute_path(std::string(image)));
  } catch (...) {
    std::cout << "Tileset " << path << " is invalid!" << std::endl;
    this->tilesets.push_back("assets/debug/missing.png");
    return;
  }
}

void Level::init() {
  if (!this->loaded)
    return;

  for (const LevelBackgroundTile tile : this->background) {
    Entity *ent = this->ent_manager->ent_create();
    ent->set_ent_class(EntClass::BACKGROUND);
    ent->set_texture_path(this->tilesets.at(tile.tileset_id - 1));
    ent->set_pos(vector_make3(tile.x, tile.y, 0.0f));
    ent->set_active(true);
  }

  for (const LevelObject obj : this->objects) {
    /* todo: object spawning */
  }

  for (const LevelCollisionTile tile : this->collisions) {
    Entity *ent = this->ent_manager->ent_create();
    ent->set_ent_class(EntClass::CLIP);
    ent->set_texture_path("assets/debug/collide_32x32.png");
    ent->set_pos(vector_make3(tile.x, tile.y, 0.0f));
    ent->set_visible(false);
    ent->set_active(true);
  }
}

void Level::update() {}
