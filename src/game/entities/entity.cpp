#include "entity.h"

#include "../vector.h"

#include <iostream>

bool Entity::is_valid() const { return this->entity_index > 0 && this->active && !this->should_remove; }

bool Entity::collides_with(Entity *ent) const {
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

void Entity::set_parent(Entity *ent) { this->parent = ent; }

void Entity::rotate_continuous(const Vector3 axis, const f32 angular_velocity) {
  this->axis             = axis;
  this->angular_velocity = angular_velocity;
}

bool Entity::should_be_removed() const { return this->should_remove; }

Vector3 Entity::get_aabb_center() const { return (this->aabb_max + this->aabb_min) / 2; }

Vector3 Entity::get_aabb_center_absolute() const {
  return (this->aabb_max + this->aabb_min) / 2 * this->scale.x + this->pos;
}

Vector3 Entity::get_aabb_min_absolute() const { return this->aabb_min * this->scale.x + this->pos; }

Vector3 Entity::get_aabb_max_absolute() const { return this->aabb_max * this->scale.x + this->pos; }

void Entity::set_default() {
  const size_t addr     = (size_t)this;
  this->id              = -1;
  this->current_anim    = 0;
  this->ready           = false;
  this->active          = false;
  this->animated        = false;
  this->should_remove   = false;
  this->should_sort     = false;
  this->should_collide  = false;
  this->should_update   = true;
  this->visible         = true;
  this->ent_class       = EntClass::UNKNOWN;
  this->renderable      = nullptr;
  this->anims           = nullptr;
  this->uv_size         = { 1.0f, 1.0f };
  this->scale           = { 1.0f, 1.0f };
  this->uv_offset       = { 0.0f, 0.0f };
  this->pos             = { 0.0f, 0.0f, 0.0f };
  this->velocity        = { 0.0f, 0.0f, 0.0f };
  this->aabb_min        = { -0.05f, -0.05f, 0.0f };
  this->aabb_max        = { 0.05f, 0.05f, 0.0f };
  this->axis            = { 0.0f, 0.0f, 1.0f };
  this->color           = { 1.0f, 1.0f, 1.0f, 1.0f };
  this->rotation_matrix = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
  };
  this->angle            = 0.0f;
  this->angular_velocity = 0.0f;
  this->updated_at       = 0.0;
  this->parent           = nullptr;
  this->texture_path     = "";
  this->lifetime         = 0.0;
  this->created_at       = platform_time_f64();
}
