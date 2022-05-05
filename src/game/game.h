#ifndef __GAME_GAME_H
#define __GAME_GAME_H

#define MAX_ENTITIES 4096

#include "entity.h"

#include <engine/input/input.h>
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <memory>
#include <vector>

class Game {
private:
  std::vector<Entity *> entities;
  Entity *player;
  InputState *input_state;

public:
  Game() {
    this->input_state = nullptr;
    this->player      = nullptr;
  }

  ~Game() {
    for (Entity *ent : this->entities)
      delete ent;
  }

  /* Events called from the engine */
  void update(GameState *state, mutex_t *lock);
  void update_lazy(GameState *state, mutex_t *lock);
  void update_paused(GameState *state, mutex_t *lock);
  void update_renderables(
    GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
  );
  void create_bindings(GameState *state, mutex_t *lock, InputState *input_state);

  /* Input stuff */
  void process_input(GameState *state);

  /* Other stuff */
  Entity *ent_create(Entity *parent = nullptr);
};

#endif
