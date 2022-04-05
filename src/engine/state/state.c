#include "state.h"

GameState *game_new_state() {
  GameState *const state = (GameState *)malloc(sizeof(GameState));

  state->flags = 0;
  state->tick = 0;
  state->tickrate = 64;

  return state;
}

void game_add_flag(GameState *state, const uint32_t flag) {
  state->flags = state->flags | flag;
}

uint8_t game_is_paused(GameState *state) {
  return state->flags & GS_PAUSED;
}

uint8_t game_is_debug(GameState *state) {
  return state->flags & GS_DEBUG;
}

uint8_t game_should_exit(GameState *state) {
  return state->flags & GS_EXIT;
}
