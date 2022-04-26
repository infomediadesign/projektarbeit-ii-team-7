#include "logic.h"

#include <game/game.h>

int logic_perform(void *args) {
  ThreadData *const td = (ThreadData *)args;
  GameState *const state = (GameState *)td->state;
  mutex_t *const lock = (mutex_t *)td->lock;

  const i64 delay = 1000000 / state->tickrate;
  i64 start_time, end_time;

  while (!game_should_exit(state)) {
    start_time = platform_time_usec();

    if (game_is_paused(state)) {
      game_paused_tick(state, lock);

      platform_sleep(50);

      continue;
    }

    game_tick(state, lock);

    if (state->tick % 16 == 0)
      game_lazy_tick(state, lock);

    state->tick++;

    end_time = platform_time_usec();

    if ((end_time - start_time) < delay)
      platform_usleep(delay - (end_time - start_time));
  }

  return 0;
}
