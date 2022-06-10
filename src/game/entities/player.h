#ifndef __GAME_ENTITIES_PLAYER_H
#define __GAME_ENTITIES_PLAYER_H

#include "entity.h"

class Player {
private:
  std::shared_ptr<Entity> base;

public:
  Player(std::shared_ptr<Entity> base) {
    this->base = base;
    this->base->set_ent_class(EntClass::PLAYER);
  }

  static std::shared_ptr<Player> make(std::shared_ptr<Entity> base) { return std::make_shared<Player>(base); }

  ENT_GETTER(std::shared_ptr<Entity>, base)
};

#endif
