#ifndef __GAME_ENTITY_H
#define __GAME_ENTITY_H

#include <engine/crc64.h>
#include <engine/types/matrix.h>
#include <engine/types/numeric.h>
#include <engine/types/quaternion.h>
#include <engine/types/vector.h>
#include <engine/util.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Entity {
private:
  Matrix4 rotation_matrix;
  Quaternion quaternion;
  Vector3 position;
  Vector3 velocity;
  Vector3 velocity_rotated;
  Vector3 axis;
  Vector2 scale;
  std::shared_ptr<Entity> parent;
  std::vector<Entity> attachments;
  std::string texture_path;
  i64 id;
  f64 updated_at;
  f64 created_at;
  f64 lifetime;
  u32 renderable_id;
  u32 entity_index;
  f32 angle;
  f32 angular_velocity;
  bool ready;
  bool active;
  bool should_remove;

  void calc_rotated_velocity();

public:
  Entity() : Entity(0U) {}

  Entity(const u32 entity_index) {
    const size_t addr      = (size_t)this;
    this->id               = crc64(&addr, sizeof(size_t));
    this->entity_index     = entity_index;
    this->ready            = false;
    this->active           = false;
    this->should_remove    = false;
    this->renderable_id    = 0U;
    this->scale            = { 1.0f, 1.0f };
    this->position         = { 0.0f, 0.0f, 0.0f };
    this->velocity         = { 0.0f, 0.0f, 0.0f };
    this->velocity_rotated = { 0.0f, 0.0f, 0.0f };
    this->axis             = { 0.0f, 0.0f, 1.0f };
    this->rotation_matrix  = {
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

  const i64 get_id() const;
  const bool is_active() const;
  const bool is_ready() const;
  const u32 get_renderable_id() const;
  const Vector3 get_pos() const;
  const std::string get_texture_path() const;
  const f32 get_angle() const;
  const Vector3 get_axis() const;
  const Vector3 get_velocity() const;
  const Vector3 get_velocity_rotated() const;
  const f32 get_angular_velocity() const;
  const f64 get_updated_at() const;
  const u32 get_entity_index() const;
  const bool should_be_removed() const;
  const Vector2 get_scale() const;
  void set_active(const bool state);
  void update(const f64 current_time);
  void rotate(const Vector3 axis, const f32 angle);
  void rotate_continuous(const Vector3 axis, const f32 angular_velocity);
  void set_velocity(const Vector3 velocity);
  void set_velocity_x(const f32 velocity);
  void set_velocity_y(const f32 velocity);
  void set_parent(std::shared_ptr<Entity> ent);
  void set_texture_path(const std::string path);
  void set_renderable_id(const u32 renderable_id);
  void set_ready(const bool ready);
  void set_lifetime(const f64 lifetime);
  void set_pos(const Vector3 position);
  void set_should_remove(const bool should_remove);
  void set_scale(const Vector2 scale);
};

#endif
