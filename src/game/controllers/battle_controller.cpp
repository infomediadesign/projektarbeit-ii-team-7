#include "battle_controller.h"

void BattleController::init(GameState *state) {}

void BattleController::update(GameState *state, mutex_t *lock) {}

void BattleController::update_lazy(GameState *state, mutex_t *lock) {}

void BattleController::update_paused(GameState *state, mutex_t *lock) {}

void BattleController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {}

void BattleController::destroy(GameState *state) {}

void BattleController::process_input(GameState *state, const f64 update_time) {}
