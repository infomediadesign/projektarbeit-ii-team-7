#include "entity_manager.h"

Entity *EntityManager::ent_create(Entity *parent) {
  Entity &ent = this->find_free_ent();

  ent.set_default();
  ent.set_parent(parent);

  return &ent;
}

void EntityManager::ent_remove(Entity *ent) {
  this->dangling_renderables.push_back(ent->get_renderable());

  ent->set_should_remove(true);
  ent->set_active(false);
  ent->set_visible(false);
}

void EntityManager::create_player() {
  this->player = Player::make(this->ent_create());
  this->player->get_base()->set_active(true);
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
  for (Entity &target : this->entities) {
    if (!target.is_valid())
      continue;

    if (target.get_ent_class() != EntClass::PLAYER)
      this->ent_remove(&target);
  }
}

Renderable *
  EntityManager::ent_assign_renderable(Renderable **renderables, const u32 renderables_count, Entity *ent) const {
  for (u32 i = 0; i < renderables_count; i++) {
    if (renderables[i]->assigned_to == -1) {
      renderable_set_assigned(renderables[i], ent->get_id());
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
