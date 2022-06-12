#ifndef __GAME_ENTITIES_ENTITY_MANAGER_H
#define __GAME_ENTITIES_ENTITY_MANAGER_H

#include "entity.h"
#include "player.h"

#include <engine/render/renderable.h>
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
  bool is_valid(std::shared_ptr<Entity> ent) const;
  bool can_delete_renderable(const u32 renderable_id);
  void clear_entities();
  u32 ent_assign_renderable(Renderable *renderables, const u32 renderables_count, std::shared_ptr<Entity> ent) const;
  std::shared_ptr<Player> get_player();
  std::shared_ptr<Entity> get_player_ent();
};

#endif
