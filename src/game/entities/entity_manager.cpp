#include "entity_manager.h"

std::shared_ptr<Entity> EntityManager::ent_create(std::shared_ptr<Entity> parent) {
  std::shared_ptr<Entity> ent = std::make_shared<Entity>((u32)this->entities.size());
  ent->set_parent(parent);

  this->entities.push_back(ent);

  return ent;
}

void EntityManager::ent_remove(std::shared_ptr<Entity> ent) {
  this->dangling_renderables.push_back(ent->get_renderable_id());

  ent->set_should_remove(true);
  ent->set_active(false);
}

void EntityManager::create_player() {
  this->player = std::make_shared<Player>(this->ent_create());
  this->player->get_base()->set_texture_path("assets/debug/player_16x16.png");
  this->player->get_base()->set_active(true);
}

bool EntityManager::is_valid(std::shared_ptr<Entity> ent) const { return ent.get() != nullptr && ent->is_valid(); }

bool EntityManager::can_delete_renderable(const u32 renderable_id) {
  for (std::shared_ptr<Entity> ent : this->entities)
    if (this->is_valid(ent) && ent->get_renderable_id() == renderable_id)
      return false;

  return true;
}

void EntityManager::clear_entities() {
  for (std::shared_ptr<Entity> target : this->entities) {
    if (!this->is_valid(target))
      continue;

    if (target->get_ent_class() != EntClass::PLAYER)
      this->ent_remove(target);
  }
}

u32 EntityManager::ent_assign_renderable(
  Renderable *renderables, const u32 renderables_count, std::shared_ptr<Entity> ent
) const {
  for (u32 i = 0; i < renderables_count; i++) {
    if (renderables[i].assigned_to == -1) {
      renderable_set_assigned(&renderables[i], ent->get_id());
      return i;
    }
  }

  throw std::runtime_error("No more free renderables.");
}

std::shared_ptr<Player> EntityManager::get_player() { return this->player; }

std::shared_ptr<Entity> EntityManager::get_player_ent() {
  if (this->player != nullptr)
    return this->player->get_base();

  return nullptr;
}
