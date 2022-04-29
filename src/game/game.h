#ifndef __GAME_GAME_H
#define __GAME_GAME_H

#define MAX_ENTITIES 8196

#include "entity.h"
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types.h>

class Game {
private:
  Entity *entities;
  u32 entity_count;

public:
  Game() {
    this->entities = new Entity[MAX_ENTITIES];
    this->entity_count = 0;
  }

  ~Game() { delete[] this->entities; }

  /* Events called from the engine */
  void update(GameState *state, mutex_t *lock);
  void update_lazy(GameState *state, mutex_t *lock);
  void update_paused(GameState *state, mutex_t *lock);
  void update_renderables(GameState *state, mutex_t *lock,
                          Renderable *renderables, const u32 renderables_count);
};

#endif
