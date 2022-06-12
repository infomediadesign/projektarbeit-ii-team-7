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
