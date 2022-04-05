#include "logic.h"

int logic_perform(void *args) {
  ThreadData *td = (ThreadData *)args;
  GameState *state = (GameState *)td->state;

  while (!game_should_exit(state)) {
    if (game_is_paused(state)) {
      platform_sleep(50);

      continue;
    }

    state->tick++;

    platform_sleep(50);
  }

  return 0;
}
