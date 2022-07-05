#ifndef __GAME_ANIMATION_H
#define __GAME_ANIMATION_H

#include <engine/types/numeric.h>
#include <engine/types/vector.h>

struct Animation {
  f64 next_frame_at;
  f64 frame_delay;
  Vector2 *frames;
  u32 current_frame;
  u32 frame_count;
  bool loop;

  static inline Animation make_default();
  static inline Animation make(Vector2 *frames, const u32 frame_count, const f64 frame_delay, const bool loop);
  void advance(const f64 current_time);
  Vector2 get_current_frame() const;
};

#endif
