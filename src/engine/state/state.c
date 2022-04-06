#include "state.h"

GameState *game_new_state() {
  return (GameState *)malloc(sizeof(GameState));
}

GameState *game_default_state() {
  GameState *state = game_new_state();

  state->flags = 0;
  state->tick = 0;
  state->tickrate = 64;
  state->width = 800;
  state->height = 600;

  return state;
}

void game_add_flag(GameState *state, const uint32_t flag) {
  state->flags = state->flags | flag;
}

uint8_t game_is_paused(GameState *state) { return state->flags & GS_PAUSED; }
uint8_t game_is_debug(GameState *state) { return state->flags & GS_DEBUG; }
uint8_t game_should_exit(GameState *state) { return state->flags & GS_EXIT; }
uint8_t game_is_fullscreen(GameState *state) {
  return state->flags & GS_FULLSCREEN;
}
