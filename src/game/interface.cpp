#include "interface.h"
#include "game.h"

static Game *GAME;

void game_initialize(GameState *state) { GAME = new Game; }

void game_tick(GameState *state, mutex_t *lock) { GAME->update(state, lock); }

void game_lazy_tick(GameState *state, mutex_t *lock) {
  GAME->update_lazy(state, lock);
}

void game_paused_tick(GameState *state, mutex_t *lock) {
  GAME->update_paused(state, lock);
}

void game_adjust_renderables(GameState *state, mutex_t *lock,
                             Renderable *renderables,
                             const u32 renderables_count) {
  GAME->update_renderables(state, lock, renderables, renderables_count);
}
