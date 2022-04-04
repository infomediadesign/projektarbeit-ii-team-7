#include "state.h"

void game_add_flag(GameState *state, const uint32_t flag) {
  state->flags = state->flags | flag;
}

uint8_t game_is_paused(GameState *state) {
  return state->flags & GameFlag::PAUSED;
}

uint8_t game_is_debug(GameState *state) {
  return state->flags & GameFlag::DEBUG;
}

uint8_t game_should_exit(GameState *state) {
  return state->flags & GameFlag::EXIT;
}
