#include "dungeon_controller.h"

void DungeonController::init(GameState *state) {}

void DungeonController::update(GameState *state, mutex_t *lock) {}

void DungeonController::update_lazy(GameState *state, mutex_t *lock) {}

void DungeonController::update_paused(GameState *state, mutex_t *lock) {}

void DungeonController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {}

void DungeonController::destroy(GameState *state) {}

void DungeonController::process_input(GameState *state, const f64 update_time) {}
