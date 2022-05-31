#ifndef __GAME_ENTITY_H
#define __GAME_ENTITY_H

#include <cstdlib>
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

#define ENT_GETTER(t, f) \
  t get_##f() const { return this->f; }
#define ENT_SETTER(t, f)                    \
  void set_##f(const t f) {                 \
    this->f          = f;                   \
    this->updated_at = platform_time_f64(); \
  }

enum EntClass { UNKNOWN, PLAYER };

class Entity {
private:
  Matrix4 rotation_matrix;
  Quaternion quaternion;
  Vector3 pos;
  Vector3 velocity;
  Vector3 velocity_rotated;
  Vector3 aabb_min;
  Vector3 aabb_max;
  Vector3 axis;
  Vector2 scale;
  std::shared_ptr<Entity> parent;
  std::vector<std::shared_ptr<Entity>> attachments;
  std::string texture_path;
  i64 id;
  f64 updated_at;
  f64 created_at;
  f64 lifetime;
  EntClass ent_class;
  u32 renderable_id;
  u32 entity_index;
  f32 angle;
  f32 angular_velocity;
  bool ready;
  bool active;
  bool should_remove;
  bool should_update;

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
    this->should_update    = true;
    this->ent_class        = EntClass::UNKNOWN;
    this->renderable_id    = 0U;
    this->scale            = { 1.0f, 1.0f };
    this->pos              = { 0.0f, 0.0f, 0.0f };
    this->velocity         = { 0.0f, 0.0f, 0.0f };
    this->velocity_rotated = { 0.0f, 0.0f, 0.0f };
    this->aabb_min         = { -0.05f, -0.05f, 0.0f };
    this->aabb_max         = { 0.05f, 0.05f, 0.0f };
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

  ENT_GETTER(i64, id)
  ENT_GETTER(bool, active)
  ENT_GETTER(bool, ready)
  ENT_GETTER(u32, renderable_id)
  ENT_GETTER(Vector3, pos)
  ENT_GETTER(std::string, texture_path)
  ENT_GETTER(f32, angle)
  ENT_GETTER(Vector3, axis)
  ENT_GETTER(Vector3, velocity)
  ENT_GETTER(Vector3, velocity_rotated)
  ENT_GETTER(f32, angular_velocity)
  ENT_GETTER(f64, updated_at)
  ENT_GETTER(u32, entity_index)
  ENT_GETTER(Vector2, scale)
  ENT_GETTER(Vector3, aabb_min)
  ENT_GETTER(Vector3, aabb_max)
  ENT_GETTER(EntClass, ent_class)

  ENT_SETTER(bool, active)
  ENT_SETTER(std::string, texture_path)
  ENT_SETTER(u32, renderable_id)
  ENT_SETTER(bool, ready)
  ENT_SETTER(f64, lifetime)
  ENT_SETTER(Vector3, pos)
  ENT_SETTER(bool, should_remove)
  ENT_SETTER(Vector2, scale)
  ENT_SETTER(EntClass, ent_class)

  const bool should_be_removed() const;
  const bool collides_with(std::shared_ptr<Entity> ent) const;
  const bool is_valid() const;
  void update(const f64 current_time);
  void rotate(const Vector3 axis, const f32 angle);
  void rotate_continuous(const Vector3 axis, const f32 angular_velocity);
  void set_velocity(const Vector3 velocity);
  void set_velocity_x(const f32 velocity);
  void set_velocity_y(const f32 velocity);
  void set_parent(std::shared_ptr<Entity> ent);
};

#endif
