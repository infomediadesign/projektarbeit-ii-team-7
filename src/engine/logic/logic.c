#include "logic.h"

int logic_perform(void *args) {
  ThreadData *const td = (ThreadData *)args;
  GameState *const state = (GameState *)td->state;

  const uint32_t delay = 1000 / state->tickrate;
  struct timeval start_time, end_time;

  while (!game_should_exit(state)) {
    gettimeofday(&start_time, NULL);

    if (game_is_paused(state)) {
      platform_sleep(50);

      continue;
    }

    state->tick++;

    gettimeofday(&end_time, NULL);

    if ((end_time.tv_usec - start_time.tv_usec) < delay * 1000) {
      platform_sleep(delay - (end_time.tv_usec - start_time.tv_usec) / 1000);
    }
  }

  return 0;
}
