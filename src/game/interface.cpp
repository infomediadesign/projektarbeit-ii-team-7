#include "interface.h"

#include "game.h"

static Game *GAME;

void game_initialize(GameState *state) {
  GAME = new Game;

  GAME->init(state);
}

void game_tick(GameState *state, mutex_t *lock) { GAME->update(state, lock); }

void game_lazy_tick(GameState *state, mutex_t *lock) { GAME->update_lazy(state, lock); }

void game_paused_tick(GameState *state, mutex_t *lock) { GAME->update_paused(state, lock); }

void game_adjust_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
) {
  GAME->update_renderables(state, lock, render_state, renderables, renderables_count);
}

void game_create_bindings(GameState *state, mutex_t *lock, InputState *input_state) {
  GAME->create_bindings(state, lock, input_state);
}
