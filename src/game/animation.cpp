#include "animation.h"

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
