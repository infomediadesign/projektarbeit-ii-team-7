#ifndef __ENGINE_UTIL_H
#define __ENGINE_UTIL_H

#include "state/state.h"

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

#define DEBUG_MESSAGE(args...) if (game_is_debug(state)) { printf("[Debug] "); printf(args); }
#endif
