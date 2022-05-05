#ifndef __GAME_ENTITY_H
#define __GAME_ENTITY_H

#include <engine/types/matrix.h>
#include <engine/types/numeric.h>
#include <engine/types/quaternion.h>
#include <engine/types/vector.h>
#include <engine/util.h>
#include <string>
#include <vector>

class Entity {
private:
  Matrix4 rotation_matrix;
  Quaternion quaternion;
  Vector3 position;
  Vector3 velocity;
  Vector3 axis;
  Entity *parent;
  std::vector<Entity> attachments;
  std::string texture_path;
  f64 updated_at;
  u32 renderable_id;
  f32 angle;
  f32 angular_speed;
  bool ready;
  bool active;

public:
  Entity() : Entity(0U) {}

  Entity(const u32 renr_id) {
    this->active          = false;
    this->renderable_id   = renr_id;
    this->position        = { 0.0f, 0.0f, 0.0f };
    this->velocity        = { 0.0f, 0.0f, 0.0f };
    this->axis            = { 0.0f, 0.0f, 1.0f };
    this->rotation_matrix = {
      { 1.0f, 0.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
    };
    this->angle         = 0.0f;
    this->angular_speed = 0.0f;
    this->updated_at    = 0.0;
    this->parent        = nullptr;
    this->texture_path  = "";
  }

  const bool is_active() const;
  const u32 get_renderable_id() const;
  const Vector3 get_pos() const;
  const std::string get_texture_path() const;
  const f32 get_angle() const;
  const Vector3 get_axis() const;
  void set_active(const bool state);
  void update(const f64 current_time);
  void rotate(const Vector3 axis, const f32 angle);
  void rotate_continuous(const Vector3 axis, const f32 angular_speed);
  void set_velocity(const Vector3 velocity);
  void set_velocity_x(const f32 velocity);
  void set_velocity_y(const f32 velocity);
  void set_parent(Entity *ent);
  void set_texture_path(std::string path);
};

#endif
