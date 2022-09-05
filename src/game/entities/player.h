#ifndef __GAME_ENTITIES_PLAYER_H
#define __GAME_ENTITIES_PLAYER_H

#include "entity.h"

enum {
  PLY_ANIM_IDLE_FRONT = 0,
  PLY_ANIM_IDLE_BACK  = 1,
  PLY_ANIM_IDLE_LEFT  = 2,
  PLY_ANIM_IDLE_RIGHT = 3,
  PLY_ANIM_WALK_FRONT = 4,
  PLY_ANIM_WALK_BACK  = 5,
  PLY_ANIM_WALK_LEFT  = 6,
  PLY_ANIM_WALK_RIGHT = 7
};

class Player {
private:
  Entity *base;

public:
  Player(Entity *base) {
    this->base = base;
    this->base->set_ent_class(EntClass::PLAYER);
  }

  static Player *make(Entity *base);

  void update_anim();

  ENT_GETTER(Entity *, base)
};

#endif
