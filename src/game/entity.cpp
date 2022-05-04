#include "entity.h"

void Entity::set_active(const bool state) { this->active = state; }

void Entity::update(const f64 current_time) {
  vector_add3i(&this->position, vector_scale3(this->velocity, current_time - this->updated_at));

  this->updated_at = current_time;
}
