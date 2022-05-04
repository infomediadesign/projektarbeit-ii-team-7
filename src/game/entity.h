#ifndef __GAME_ENTITY_H
#define __GAME_ENTITY_H

#include <engine/types/numeric.h>
#include <engine/types/vector.h>
#include <engine/util.h>
#include <vector>

class Entity {
private:
  u32 renderable_id;
  Vector3 position;
  Vector3 velocity;
  Vector3 axis;
  f32 angle;
  f64 updated_at;
  bool active;
  Entity *parent;
  std::vector<Entity> attachments;

public:
  Entity() : Entity(0U) {}

  Entity(const u32 renr_id) {
    this->active        = false;
    this->renderable_id = renr_id;
    this->position      = { 0.0f, 0.0f, 0.0f };
    this->velocity      = { 0.0f, 0.0f, 0.0f };
    this->axis          = { 0.0f, 0.0f, 0.0f };
    this->angle         = 0.0f;
    this->updated_at    = 0.0;
    this->parent        = nullptr;
  }

  bool is_active();
  void set_active(const bool state);
  void update(const f64 current_time);
};

#endif
