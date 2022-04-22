#include "state.h"

GameState *game_new_state() { return (GameState *)malloc(sizeof(GameState)); }

GameState *game_default_state() {
  GameState *state = game_new_state();

  state->flags = 0U;
  state->tick = 0U;
  state->tickrate = 64U;
  state->width = 800U;
  state->height = 600U;
  state->fps_max = 300U;

  return state;
}

void game_add_flag(GameState *restrict state, const u32 flag) {
  state->flags = state->flags | flag;
}

u8 game_is_paused(const GameState *restrict state) {
  return state->flags & GS_PAUSED;
}

u8 game_is_debug(const GameState *restrict state) {
  return state->flags & GS_DEBUG;
}

u8 game_should_exit(const GameState *restrict state) {
  return state->flags & GS_EXIT;
}

u8 game_is_fullscreen(const GameState *restrict state) {
  return state->flags & GS_FULLSCREEN;
}
