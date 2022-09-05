#ifndef __GAME_GAME_H
#define __GAME_GAME_H

#define GAME_GETTER(t, n) \
  t get_##n() const { return this->n; }
#define LUA_EVENT_RUN(L, id)  \
  lua_getglobal(L, "event");  \
  lua_getfield(L, -1, "run"); \
  lua_remove(L, -2);          \
  lua_pushstring(L, id);
#define LUA_EVENT_CALL(L, args, res) lua_call(L, args + 1, res);

#include "controllers/battle_controller.h"
#include "controllers/dungeon_controller.h"
#include "controllers/menu_controller.h"
#include "controllers/overworld_controller.h"
#include "entities/entity.h"
#include "entities/entity_manager.h"
#include "entities/player.h"
#include "level.h"
#include "lua/entity.h"

#include <engine/input/input.h>
#include <engine/limits.h>
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <lua.hpp>
#include <memory>
#include <vector>

enum GameStage { GS_MENU, GS_OVERWORLD, GS_DUNGEON, GS_BATTLE, GS_UNKNOWN };

class Game {
private:
  EntityManager ent_manager;
  std::shared_ptr<Level> level;
  GameState *game_state;
  InputState *input_state;
  lua_State *lua;
  OverworldController *overworld_controller;
  DungeonController *dungeon_controller;
  BattleController *battle_controller;
  MenuController *menu_controller;
  GameStage stage;
  f32 scale;
  bool locked;

public:
  Game() {
    this->input_state          = nullptr;
    this->game_state           = nullptr;
    this->overworld_controller = nullptr;
    this->dungeon_controller   = nullptr;
    this->battle_controller    = nullptr;
    this->menu_controller      = nullptr;
    this->scale                = RENDER_SCALE;
    this->stage                = GameStage::GS_UNKNOWN;
    this->locked               = false;
  }

  GAME_GETTER(InputState *, input_state);
  GAME_GETTER(GameStage, stage);

  /* Events called from the engine */
  void init(GameState *state);
  void update(GameState *state, mutex_t *lock);
  void update_lazy(GameState *state, mutex_t *lock);
  void update_paused(GameState *state, mutex_t *lock);
  void update_renderables(
    GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
  );
  void create_bindings(GameState *state, mutex_t *lock, InputState *input_state);
  void process_input(GameState *state, const f64 update_time);
  void set_stage(const GameStage stage);
  void init_lua();

  static i32 compare_renderables(const void *r1, const void *r2);
  static i32 lua_set_stage(lua_State *state);
  static i32 lua_bind(lua_State *state);
  static i32 lua_get_stage(lua_State *state);
  static i32 lua_player(lua_State *state);
  static i32 lua_ent_create(lua_State *state);
};

#endif
