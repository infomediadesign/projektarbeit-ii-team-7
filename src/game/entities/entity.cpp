#include "entity.h"

#include "../vector.h"

#include <iostream>

bool Entity::is_valid() const { return this->active && !this->should_remove && this->ready; }

bool Entity::collides_with(std::shared_ptr<Entity> ent) const {
  const Vector3 target_min = ent->get_aabb_min() * ent->get_scale().x + ent->get_pos();
  const Vector3 target_max = ent->get_aabb_max() * ent->get_scale().x + ent->get_pos();
  const Vector3 aabb_min   = this->aabb_min * this->scale.x + this->pos;
  const Vector3 aabb_max   = this->aabb_max * this->scale.x + this->pos;

  if (aabb_min.x > target_max.x || aabb_max.x < target_min.x)
    return false;

  if (aabb_min.y > target_max.y || aabb_max.y < target_min.y)
    return false;

  return true;
}

void Entity::update(const f64 current_time) {
  const f64 delta = current_time - this->updated_at;

  if (lifetime > 0.0 && this->created_at + lifetime <= current_time) {
    this->set_active(false);
    this->should_remove = true;

    return;
  }

  if (!this->should_update)
    return;

  if (this->angular_velocity != 0.0f) {
    this->angle += this->angular_velocity * delta;
    this->rotate(this->axis, this->angle);
  }

  vector_add3i(&this->pos, this->velocity * delta);

  if (this->animated && this->anims != nullptr) {
    this->anims[this->current_anim].advance(current_time);
    this->uv_offset = this->anims[this->current_anim].get_current_frame();
  }

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

void Entity::set_parent(std::shared_ptr<Entity> ent) { this->parent = ent; }

void Entity::rotate_continuous(const Vector3 axis, const f32 angular_velocity) {
  this->axis             = axis;
  this->angular_velocity = angular_velocity;
}

bool Entity::should_be_removed() const { return this->should_remove; }
