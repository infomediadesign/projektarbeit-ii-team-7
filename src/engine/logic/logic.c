#include "logic.h"

int logic_perform(void *args) {
  ThreadData *const td = (ThreadData *)args;
  GameState *const state = (GameState *)td->state;

  const i64 delay = 1000 / state->tickrate;
  i64 start_time, end_time;

  while (!game_should_exit(state)) {
    start_time = platform_time_usec();

    if (game_is_paused(state)) {
      platform_sleep(50);

      continue;
    }

    state->tick++;

    end_time = platform_time_usec();

    if ((end_time - start_time) < delay * 1000) {
      platform_sleep(delay - (end_time - start_time) / 1000);
    }
  }

  return 0;
}
