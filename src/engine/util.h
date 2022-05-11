#ifndef __ENGINE_UTIL_H
#define __ENGINE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#define LERP_FUNC(T)                                                         \
  static inline T util_lerp_##T(const f32 delta, const T from, const T to) { \
    if (delta >= 1.0f)                                                       \
      return to;                                                             \
    else if (delta <= 0.0f)                                                  \
      return from;                                                           \
    return (T)(from + (to - from) * delta);                                       \
  }

#include "state/state.h"
#include "types/numeric.h"

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

#define DEBUG_MESSAGE(args)   \
  if (game_is_debug(state)) { \
    printf("[Debug] ");       \
    printf(args);             \
  }

static inline u64 util_kibibytes(const u64 kb) { return kb * 1024; }

static inline u64 util_mebibytes(const u64 mb) { return mb * 1024 * 1024; }

static inline u64 util_gibibytes(const u64 gb) { return gb * 1024 * 1024 * 1024; }

static inline u64 util_kbytes(const u64 kb) { return kb * 1000; }

static inline u64 util_mbytes(const u64 mb) { return mb * 1000000; }

static inline u64 util_gbytes(const u64 gb) { return gb * 1000000000; }

static inline f32 util_radians(const f32 deg) { return (deg * (f32)MF_PI) / 180.0f; }

static inline f32 util_degrees(const f32 rad) { return (rad * 180.0f) / (f32)MF_PI; }

static inline f32 util_meters(const f32 pixels) { return pixels * 0.001f; }

LERP_FUNC(i64)
LERP_FUNC(u64)
LERP_FUNC(f32)
LERP_FUNC(f64)

#ifdef __cplusplus
}
#endif

#endif
