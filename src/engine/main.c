#include "input/argparse.h"
#include "logic/logic.h"
#include "platform.h"
#include "state/state.h"
#include "util.h"

int dummy(void *args) {
  ThreadData *td = (ThreadData *)args;
  GameState *state = (GameState *)td->state;

  while (1) {
    platform_sleep(100);
  }

  return 0;
}

/**
 * @brief The main entry point of the engine executable
 *
 * @param argc Amount of command-line parameters passed
 * @param argv List of parameters separated by space, including executable name
 * itself
 * @return int Exit status code
 */
int main(const int argc, const char *argv[]) {
  printf("Starting...\n");

  /* Initialize game state and a mutex lock */
  GameState *const state = game_new_state();
  mutex_t state_lock;

  input_parse_args(state, argc, argv);

  if (game_is_debug(state)) {
    printf("Debug mode is enabled.\n");
  }

  /* Spawn threads */
  ThreadData thread_data = {.state = state, .lock = &state_lock};

  const thread_t render_thread = platform_spawn(dummy, &thread_data);
  const thread_t logic_thread = platform_spawn(logic_perform, &thread_data);
  const thread_t input_thread = platform_spawn(dummy, &thread_data);

  /* Wait until game state tells us we should exit */
  while (!game_should_exit(state)) {
    printf("  in main loop, tick %lu\n", state->tick);
    platform_sleep(1000);
  }

  printf("Exiting...\n");

  /* Join all threads into the main thread */
  platform_join(input_thread);
  platform_join(logic_thread);
  platform_join(render_thread);

  /* Clean up */
  free(state);

  return 0;
}
