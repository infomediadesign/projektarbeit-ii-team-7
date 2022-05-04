#include "entity.h"

void Entity::set_active(const bool state) { this->active = state; }

void Entity::update(const f64 current_time) {
  const Vector3 rotated_velocity = vector_make3(
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.x)),
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.y)),
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.z))
  );

  vector_add3i(&this->position, vector_scale3(rotated_velocity, current_time - this->updated_at));

  this->updated_at = current_time;
}

void Entity::rotate(const Vector3 axis, const f32 angle) {
  this->quaternion      = quaternion_rotation(axis, angle);
  this->rotation_matrix = quaternion_rotation_matrix(this->quaternion);
}

void Entity::set_velocity(const Vector3 velocity) { this->velocity = velocity; }

void Entity::set_velocity_x(const f32 velocity) { this->velocity.x = velocity; }

void Entity::set_velocity_y(const f32 velocity) { this->velocity.y = velocity; }
