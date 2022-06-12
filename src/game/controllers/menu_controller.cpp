#include "menu_controller.h"

void MenuController::init(GameState *state) {}

void MenuController::update(GameState *state, mutex_t *lock) {}

void MenuController::update_lazy(GameState *state, mutex_t *lock) {}

void MenuController::update_paused(GameState *state, mutex_t *lock) {}

void MenuController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {}

void MenuController::destroy(GameState *state) {}

void MenuController::process_input(GameState *state, const f64 update_time) {}
