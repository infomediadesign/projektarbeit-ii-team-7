#include "game.h"
#include <iostream>

void Game::update(GameState *state, mutex_t *lock) {}

void Game::update_lazy(GameState *state, mutex_t *lock) {}

void Game::update_paused(GameState *state, mutex_t *lock) {}

void Game::update_renderables(GameState *state, mutex_t *lock,
                              Renderable *renderables,
                              const u32 renderables_count) {}
