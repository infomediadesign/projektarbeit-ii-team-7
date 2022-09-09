#include "dungeon_controller.h"

void DungeonController::init(GameState *state) {}

void DungeonController::update(GameState *state, mutex_t *lock) {}

void DungeonController::update_lazy(GameState *state, mutex_t *lock) {}

void DungeonController::update_paused(GameState *state, mutex_t *lock) {}

void DungeonController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
) {}

void DungeonController::destroy(GameState *state) {}

void DungeonController::process_input(GameState *state, const f64 update_time) {}

void DungeonController::changelevel(const std::string level) {
  if (this->level != nullptr) {
    this->base.ent_manager->clear_entities();
    delete this->level;
    this->level = nullptr;
  }

  this->level = new Level(this->base.lua, this->base.ent_manager);
  this->level->load_json("levels/" + level + ".json");
  this->level->init();
}
