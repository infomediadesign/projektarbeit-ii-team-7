#ifndef __GAME_ENTITIES_ENTITY_H
#define __GAME_ENTITIES_ENTITY_H

#include "../animation.h"

#include <cstdlib>
#include <engine/crc64.h>
#include <engine/render/renderable.h>
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
  void set_##f(t f) {                       \
    this->f          = f;                   \
    this->updated_at = platform_time_f64(); \
  }

enum EntClass { UNKNOWN, PLAYER, BACKGROUND, CLIP, ENEMY };

class Entity {
private:
  Matrix4 rotation_matrix;
  Quaternion quaternion;
  Vector3 aabb_max;
  Vector3 aabb_min;
  Vector3 velocity;
  Vector3 axis;
  Vector3 pos;
  Vector2 uv_size;
  Vector2 uv_offset;
  Vector2 scale;
  std::shared_ptr<Entity> parent;
  std::vector<std::shared_ptr<Entity>> attachments;
  std::string texture_path;
  Renderable *renderable;
  Animation *anims;
  f64 created_at;
  f64 updated_at;
  f64 lifetime;
  i64 id;
  EntClass ent_class;
  u32 current_anim;
  u32 entity_index;
  f32 angular_velocity;
  f32 angle;
  bool should_collide;
  bool should_remove;
  bool should_update;
  bool should_sort;
  bool animated;
  bool active;
  bool ready;

public:
  Entity() : Entity(0U) {}

  Entity(const u32 entity_index) {
    const size_t addr     = (size_t)this;
    this->id              = crc64(&addr, sizeof(size_t));
    this->entity_index    = entity_index;
    this->current_anim    = 0;
    this->ready           = false;
    this->active          = false;
    this->animated        = false;
    this->should_remove   = false;
    this->should_sort     = false;
    this->should_collide  = false;
    this->should_update   = true;
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

  ENT_GETTER(std::string, texture_path);
  ENT_GETTER(Animation *, anims);
  ENT_GETTER(Vector3, aabb_max)
  ENT_GETTER(Vector3, aabb_min)
  ENT_GETTER(Vector3, velocity)
  ENT_GETTER(Vector3, axis)
  ENT_GETTER(Vector3, pos)
  ENT_GETTER(Vector2, uv_offset)
  ENT_GETTER(Vector2, uv_size)
  ENT_GETTER(Vector2, scale)
  ENT_GETTER(Renderable *, renderable)
  ENT_GETTER(f64, updated_at)
  ENT_GETTER(i64, id)
  ENT_GETTER(EntClass, ent_class)
  ENT_GETTER(f32, angular_velocity)
  ENT_GETTER(u32, current_anim)
  ENT_GETTER(u32, entity_index)
  ENT_GETTER(f32, angle)
  ENT_GETTER(bool, should_collide)
  ENT_GETTER(bool, should_sort)
  ENT_GETTER(bool, animated)
  ENT_GETTER(bool, active)
  ENT_GETTER(bool, ready)

  ENT_SETTER(Animation *, anims);
  ENT_SETTER(Vector3, pos)
  ENT_SETTER(Vector2, uv_offset)
  ENT_SETTER(Vector2, uv_size)
  ENT_SETTER(Vector2, scale)
  ENT_SETTER(std::string, texture_path)
  ENT_SETTER(Renderable *, renderable)
  ENT_SETTER(f64, lifetime)
  ENT_SETTER(u32, current_anim)
  ENT_SETTER(EntClass, ent_class)
  ENT_SETTER(bool, should_collide)
  ENT_SETTER(bool, should_remove)
  ENT_SETTER(bool, should_sort)
  ENT_SETTER(bool, animated)
  ENT_SETTER(bool, active)
  ENT_SETTER(bool, ready)

  void rotate_continuous(const Vector3 axis, const f32 angular_velocity);
  bool collides_with(std::shared_ptr<Entity> ent) const;
  void rotate(const Vector3 axis, const f32 angle);
  void set_parent(std::shared_ptr<Entity> ent);
  void set_velocity(const Vector3 velocity);
  void set_velocity_x(const f32 velocity);
  void set_velocity_y(const f32 velocity);
  void update(const f64 current_time);
  bool should_be_removed() const;
  bool is_valid() const;
};

#endif
