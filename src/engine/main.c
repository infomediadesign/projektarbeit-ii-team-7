#include "platform.h"
#include "state/state.h"

void dummy(void *args) {}

/**
 * @brief Data to be sent to logic threads.
 *
 * Simply a container for a tulpe of game state
 * and its mutex lock for thread interoperability.
 *
 */
typedef struct ThreadData {
  GameState *state;
  mutex_t *lock;
} ThreadData;

/**
 * @brief The main entry point of the engine executable
 *
 * @param argc Amount of command-line parameters passed
 * @param argv List of parameters separated by space, including executable name
 * itself
 * @return int Exit status code
 */
int main(int argc, char *argv[]) {
  /* Initialize game state and a mutex lock */
  GameState *const state = (GameState *)malloc(sizeof(GameState));
  mutex_t state_lock;

  /* Spawn threads */
  ThreadData thread_data = {.state = state, .lock = &state_lock};

  const thread_t render_thread = platform_spawn(dummy, &thread_data);
  const thread_t logic_thread = platform_spawn(dummy, &thread_data);
  const thread_t input_thread = platform_spawn(dummy, &thread_data);

  /* Wait until game state tells us we should exit */
  while (!game_should_exit(state)) {
    platform_sleep(100);
  }

  /* Join all threads into the main thread */
  platform_join(input_thread);
  platform_join(logic_thread);
  platform_join(render_thread);

  /* Clean up */
  free(state);

  return 0;
}
