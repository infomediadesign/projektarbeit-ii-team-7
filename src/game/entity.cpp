#include "entity.h"

#include <iostream>

const i64 Entity::get_id() const { return this->id; }

const bool Entity::is_active() const { return this->active; }

const bool Entity::is_ready() const { return this->ready; }

const u32 Entity::get_renderable_id() const { return this->renderable_id; }

const Vector3 Entity::get_pos() const { return this->position; }

const std::string Entity::get_texture_path() const { return this->texture_path; }

const f32 Entity::get_angle() const { return this->angle; }

const Vector3 Entity::get_axis() const { return this->axis; }

const Vector3 Entity::get_velocity() const { return this->velocity; }

const f32 Entity::get_angular_velocity() const { return this->angular_velocity; }

const f64 Entity::get_updated_at() const { return this->updated_at; }

const Vector3 Entity::get_velocity_rotated() const { return this->velocity_rotated; }

const u32 Entity::get_entity_index() const { return this->entity_index; }

const bool Entity::should_be_removed() const { return this->should_remove; }

const Vector2 Entity::get_scale() const { return this->scale; }

void Entity::set_active(const bool state) {
  this->active     = state;
  this->updated_at = platform_time_f64();
}

void Entity::update(const f64 current_time) {
  const f64 delta = current_time - this->updated_at;

  if (lifetime > 0.0 && this->created_at + lifetime <= current_time) {
    this->set_active(false);
    this->should_remove = true;
  }

  if (this->angular_velocity != 0.0f) {
    this->angle += this->angular_velocity * delta;
    this->rotate(this->axis, this->angle);
  }

  vector_add3i(&this->position, vector_scale3(this->velocity_rotated, delta));

  this->updated_at = current_time;
}

void Entity::rotate(const Vector3 axis, const f32 angle) {
  this->axis            = axis;
  this->angle           = angle;
  this->quaternion      = quaternion_rotation(axis, angle);
  this->rotation_matrix = quaternion_rotation_matrix(this->quaternion);

  this->calc_rotated_velocity();
}

void Entity::set_velocity(const Vector3 velocity) {
  this->velocity = velocity;

  this->calc_rotated_velocity();
}

void Entity::set_velocity_x(const f32 velocity) {
  this->velocity.x = velocity;

  this->calc_rotated_velocity();
}

void Entity::set_velocity_y(const f32 velocity) {
  this->velocity.y = velocity;

  this->calc_rotated_velocity();
}

void Entity::set_parent(std::shared_ptr<Entity> ent) { this->parent = ent; }

void Entity::set_texture_path(const std::string path) { this->texture_path = path; }

void Entity::rotate_continuous(const Vector3 axis, const f32 angular_velocity) {
  this->axis             = axis;
  this->angular_velocity = angular_velocity;
}

void Entity::calc_rotated_velocity() {
  this->velocity_rotated = vector_make3(
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.x)),
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.y)),
    vector_dot3(this->velocity, vector_from_matrix_comp3(this->rotation_matrix.z))
  );
}

void Entity::set_renderable_id(const u32 renderable_id) { this->renderable_id = renderable_id; }

void Entity::set_ready(const bool ready) { this->ready = ready; }

void Entity::set_lifetime(const f64 lifetime) { this->lifetime = lifetime; }

void Entity::set_pos(const Vector3 position) { this->position = position; }

void Entity::set_should_remove(const bool should_remove) { this->should_remove = should_remove; }

void Entity::set_scale(const Vector2 scale) { this->scale = scale; }
