#include "entity.h"

#include <iostream>

const bool Entity::is_active() const { return this->active; }

const u32 Entity::get_renderable_id() const { return this->renderable_id; }

const Vector3 Entity::get_pos() const { return this->position; }

const std::string Entity::get_texture_path() const { return this->texture_path; }

const f32 Entity::get_angle() const { return this->angle; }

const Vector3 Entity::get_axis() const { return this->axis; }

const Vector3 Entity::get_velocity() const { return this->velocity; }

const f32 Entity::get_angular_speed() const { return this->angular_speed; }

const f64 Entity::get_updated_at() const { return this->updated_at; }

void Entity::set_active(const bool state) {
  this->active     = state;
  this->updated_at = platform_time_f64();
}

void Entity::update(const f64 current_time) {
  if (this->angular_speed != 0.0f) {
    this->angle += this->angular_speed;
    this->rotate(this->axis, this->angle);
  }

  const Vector3 rotated_velocity = vector_make3(
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.x)),
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.y)),
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.z))
  );

  vector_add3i(&this->position, vector_scale3(rotated_velocity, current_time - this->updated_at));

  this->updated_at = current_time;
}

void Entity::rotate(const Vector3 axis, const f32 angle) {
  this->axis            = axis;
  this->angle           = angle;
  this->quaternion      = quaternion_rotation(axis, angle);
  this->rotation_matrix = quaternion_rotation_matrix(this->quaternion);
}

void Entity::set_velocity(const Vector3 velocity) { this->velocity = velocity; }

void Entity::set_velocity_x(const f32 velocity) { this->velocity.x = velocity; }

void Entity::set_velocity_y(const f32 velocity) { this->velocity.y = velocity; }

void Entity::set_parent(Entity *ent) { this->parent = ent; }

void Entity::set_texture_path(std::string path) { this->texture_path = path; }

void Entity::rotate_continuous(const Vector3 axis, const f32 angular_speed) {
  this->axis          = axis;
  this->angular_speed = angular_speed;
}
