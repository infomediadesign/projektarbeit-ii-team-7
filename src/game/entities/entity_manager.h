#ifndef __GAME_ENTITIES_ENTITY_MANAGER_H
#define __GAME_ENTITIES_ENTITY_MANAGER_H

#include "entity.h"
#include "player.h"

#include <engine/types/numeric.h>
#include <lua.hpp>
#include <memory>
#include <vector>

class EntityManager {
public:
  std::vector<std::shared_ptr<Entity>> entities;
  std::vector<u32> dangling_renderables;
  std::shared_ptr<Player> player;

  EntityManager() { this->player = nullptr; }

  std::shared_ptr<Entity> ent_create(std::shared_ptr<Entity> parent = nullptr);
  void ent_remove(std::shared_ptr<Entity> ent);
  void create_player();
};

#endif
