#ifndef __GAME_CONTROLLERS_CONTROLLER_H
#define __GAME_CONTROLLERS_CONTROLLER_H

#include "../entities/entity_manager.h"

#include <engine/input/input.h>
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <engine/types/vector.h>
#include <lua.hpp>
#include <memory>
#include <vector>

#define CONTROLLER_METHOD_DEFINITIONS                                                                                \
  void init(GameState *state);                                                                                       \
  void update(GameState *state, mutex_t *lock);                                                                      \
  void update_lazy(GameState *state, mutex_t *lock);                                                                 \
  void update_paused(GameState *state, mutex_t *lock);                                                               \
  void update_renderables(                                                                                           \
    GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count \
  );                                                                                                                 \
  void destroy(GameState *state);                                                                                    \
  Controller *get_base() { return &this->base; }                                                                     \
  void process_input(GameState *state, const f64 update_time);

class Controller {
public:
  lua_State *lua;
  EntityManager *ent_manager;
  InputState *input_state;
};

#endif
