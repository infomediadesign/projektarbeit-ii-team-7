#ifndef __GAME_GAME_H
#define __GAME_GAME_H

#define MAX_ENTITIES 4096

#include "entity.h"
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <vector>

class Game {
private:
  std::vector<Entity> entities;

public:
  Game() {}

  ~Game() { delete[] this->entities; }

  /* Events called from the engine */
  void update(GameState *state, mutex_t *lock);
  void update_lazy(GameState *state, mutex_t *lock);
  void update_paused(GameState *state, mutex_t *lock);
  void update_renderables(GameState *state, mutex_t *lock,
                          Renderable *renderables, const u32 renderables_count);
};

#endif
