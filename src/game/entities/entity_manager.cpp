#include "entity_manager.h"

#include "../lua/entity.h"
#include "../lua/helpers.h"

Entity *EntityManager::ent_create(Entity *parent) {
  Entity &ent = this->find_free_ent();

  ent.set_default();
  ent.set_parent(parent);

  LUA_EVENT_RUN(this->lua, "ent_create");
  lua_push_entity(this->lua, &ent);
  LUA_EVENT_CALL(this->lua, 1, 0);

  return &ent;
}

void EntityManager::ent_remove(Entity *ent) {
  LUA_EVENT_RUN(this->lua, "pre_ent_remove");
  lua_push_entity(this->lua, ent);
  LUA_EVENT_CALL(this->lua, 1, 0);

  LUA_EVENT_RUN(this->lua, "can_remove_entity");
  lua_push_entity(this->lua, ent);
  LUA_EVENT_CALL(this->lua, 1, 1);

  if (lua_isboolean(this->lua, -1) && lua_toboolean(this->lua, -1) == 0)
    return;

  this->dangling_renderables.push_back(ent->get_renderable());

  ent->set_should_remove(true);
  ent->set_active(false);
  ent->set_visible(false);

  LUA_EVENT_RUN(this->lua, "ent_removed");
  lua_push_entity(this->lua, ent);
  LUA_EVENT_CALL(this->lua, 1, 0);
}

void EntityManager::create_player() {
  this->player = Player::make(this->ent_create());
  this->player->get_base()->set_active(true);

  LUA_EVENT_RUN(this->lua, "player_created");
  lua_push_entity(this->lua, this->player->get_base());
  LUA_EVENT_CALL(this->lua, 1, 0);
}

bool EntityManager::can_delete_renderable(const Renderable *const renderable) {
  if (renderable == nullptr)
    return false;

  for (Entity &ent : this->entities)
    if (ent.is_valid() && ent.get_renderable() == renderable)
      return false;

  return true;
}

void EntityManager::clear_entities() {
  LUA_EVENT_RUN(this->lua, "pre_clear_entities");
  LUA_EVENT_CALL(this->lua, 0, 0);

  for (Entity &target : this->entities) {
    if (!target.is_valid())
      continue;

    if (target.get_ent_class() != EntClass::PLAYER) {
      LUA_EVENT_RUN(this->lua, "can_clear_entity");
      lua_push_entity(this->lua, &target);
      LUA_EVENT_CALL(this->lua, 1, 1);

      if (lua_isboolean(this->lua, -1) && lua_toboolean(this->lua, -1) == 0)
        continue;

      this->ent_remove(&target);
    }
  }

  LUA_EVENT_RUN(this->lua, "post_clear_entities");
  LUA_EVENT_CALL(this->lua, 0, 0);
}

Renderable *
  EntityManager::ent_assign_renderable(Renderable **renderables, const u32 renderables_count, Entity *ent) const {
  for (u32 i = 0; i < renderables_count; i++) {
    if (renderables[i]->assigned_to == -1) {
      renderable_set_assigned(renderables[i], ent->get_entity_index());
      return renderables[i];
    }
  }

  throw std::runtime_error("No more free renderables.");
}

Player *EntityManager::get_player() { return this->player; }

Entity *EntityManager::get_player_ent() {
  if (this->player != nullptr)
    return this->player->get_base();

  return nullptr;
}

Entity &EntityManager::find_free_ent() {
  for (u32 i = 1; i < MAX_ENTITIES; i++)
    if (!this->entities[i].is_valid())
      return this->entities[i];

  return this->entities[0];
}
