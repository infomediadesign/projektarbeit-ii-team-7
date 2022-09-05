#ifndef __GAME_ENTITIES_ENTITY_MANAGER_H
#define __GAME_ENTITIES_ENTITY_MANAGER_H

#include "entity.h"
#include "player.h"

#include <engine/limits.h>
#include <engine/render/renderable.h>
#include <engine/types/numeric.h>
#include <lua.hpp>
#include <memory>
#include <string.h>
#include <vector>

class EntityManager {
public:
  Entity entities[MAX_ENTITIES];
  std::vector<Renderable *> dangling_renderables;
  Player *player;
  lua_State *lua;

  EntityManager() {
    this->player = nullptr;
    this->lua    = nullptr;

    for (u32 i = 0; i < MAX_ENTITIES; i++) {
      this->entities[i].set_entity_index(i);
      this->entities[i].set_default();
    }
  }

  Entity *ent_create(Entity *parent = nullptr);
  void ent_remove(Entity *ent);
  void create_player();
  bool can_delete_renderable(const Renderable *const renderable);
  void clear_entities();
  Renderable *ent_assign_renderable(Renderable **renderables, const u32 renderables_count, Entity *ent) const;
  Player *get_player();
  Entity *get_player_ent();

private:
  Entity &find_free_ent();
};

#endif
