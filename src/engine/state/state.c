#include "state.h"

GameState *game_new_state() {
  return (GameState *)malloc(sizeof(GameState));
}

GameState *game_default_state() {
  GameState *state = game_new_state();

  state->flags = 0U;
  state->tick = 0U;
  state->tickrate = 64U;
  state->width = 800U;
  state->height = 600U;

  return state;
}

void game_add_flag(GameState *state, const u32 flag) {
  state->flags = state->flags | flag;
}

uint8_t game_is_paused(const GameState *state) { return state->flags & GS_PAUSED; }
uint8_t game_is_debug(const GameState *state) { return state->flags & GS_DEBUG; }
uint8_t game_should_exit(const GameState *state) { return state->flags & GS_EXIT; }
uint8_t game_is_fullscreen(const GameState *state) {
  return state->flags & GS_FULLSCREEN;
}
