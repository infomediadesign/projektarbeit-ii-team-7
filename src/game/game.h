#ifndef __GAME_GAME_H
#define __GAME_GAME_H

#define MAX_ENTITIES 4096

#include "entities/entity.h"
#include "level.h"

#include <engine/input/input.h>
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <lua.hpp>
#include <memory>
#include <vector>

class Game {
private:
  std::vector<std::shared_ptr<Entity>> entities;
  std::vector<u32> dangling_renderables;
  std::shared_ptr<Entity> player;
  std::shared_ptr<Level> level;
  InputState *input_state;
  lua_State *lua;

public:
  Game() {
    this->input_state = nullptr;
    this->player      = nullptr;
  }

  /* Events called from the engine */
  void init(GameState *state);
  void update(GameState *state, mutex_t *lock);
  void update_lazy(GameState *state, mutex_t *lock);
  void update_paused(GameState *state, mutex_t *lock);
  void update_renderables(
    GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
  );
  void create_bindings(GameState *state, mutex_t *lock, InputState *input_state);

  /* Input stuff */
  void process_input(GameState *state, const f64 update_time);

  /* Gameplay logic stuff */
  void check_collision(std::shared_ptr<Entity> ent);
  bool can_delete_renderable(const u32 renderable_id);
  void clear_entities();
  bool is_valid(std::shared_ptr<Entity> ent) const;

  std::shared_ptr<Entity> ent_create(std::shared_ptr<Entity> parent = nullptr);
  const u32
    ent_assign_renderable(Renderable *renderables, const u32 renderables_count, std::shared_ptr<Entity> ent) const;
  void ent_remove(std::shared_ptr<Entity> ent);
};

#endif
