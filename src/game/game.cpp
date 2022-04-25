#include "game.h"
#include <iostream>

void game_initialize(GameState *state) {}

void game_tick(GameState *state, mutex_t *lock) {}

void game_lazy_tick(GameState *state, mutex_t *lock) {}

void game_adjust_renderables(GameState *state, mutex_t *lock, Renderable *renderables, const u32 renderables_count) {}
