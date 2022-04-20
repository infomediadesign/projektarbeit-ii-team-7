#ifndef __ENGINE_UTIL_H
#define __ENGINE_UTIL_H

#include "state/state.h"
#include "types.h"

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

#define DEBUG_MESSAGE(args)                                                    \
  if (game_is_debug(state)) {                                                  \
    printf("[Debug] ");                                                        \
    printf(args);                                                              \
  }

inline u64 util_kibibytes(const u64 kb) {
  return kb * 1024;
}

inline u64 util_mebibytes(const u64 mb) {
  return mb * 1024 * 1024;
}

inline u64 util_gibibytes(const u64 gb) {
  return gb * 1024 * 1024 * 1024;
}

inline u64 util_kbytes(const u64 kb) {
  return kb * 1000;
}

inline u64 util_mbytes(const u64 mb) {
  return mb * 1000000;
}

inline u64 util_gbytes(const u64 gb) {
  return gb * 1000000000;
}

#endif
