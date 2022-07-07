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

  static inline Animation make_default() {
    return {
      .next_frame_at = 0.0f, .frame_delay = 0.1f, .frames = nullptr, .current_frame = 0, .frame_count = 0, .loop = false
    };
  }

  static inline Animation make(Vector2 *frames, const u32 frame_count, const f64 frame_delay, const bool loop) {
    return { .next_frame_at = 0.0f,
             .frame_delay   = frame_delay,
             .frames        = frames,
             .current_frame = 0,
             .frame_count   = frame_count,
             .loop          = loop };
  }

  void advance(const f64 current_time);
  Vector2 get_current_frame() const;
};

#endif
