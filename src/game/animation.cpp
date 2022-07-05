#include "animation.h"

Animation Animation::make_default() {
  return {
    .next_frame_at = 0.0f, .frame_delay = 0.1f, .frames = nullptr, .current_frame = 0, .frame_count = 0, .loop = false
  };
}

Animation Animation::make(Vector2 *frames, const u32 frame_count, const f64 frame_delay, const bool loop) {
  return { .next_frame_at = 0.0f,
           .frame_delay   = frame_delay,
           .frames        = frames,
           .current_frame = 0,
           .frame_count   = frame_count,
           .loop          = loop };
}

void Animation::advance(const f64 current_time) {
  if (current_time >= this->next_frame_at) {
    this->next_frame_at = current_time + this->frame_delay;

    if (this->current_frame >= this->frame_count - 1) {
      if (this->loop)
        this->current_frame = 0;
    } else
      this->current_frame++;
  }
}

Vector2 Animation::get_current_frame() const { return this->frames[this->current_frame]; }
