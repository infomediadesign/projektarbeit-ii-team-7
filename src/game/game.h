#ifndef __GAME_GAME_H
#define __GAME_GAME_H

#define MAX_ENTITIES 4096

#include "controllers/battle_controller.h"
#include "controllers/dungeon_controller.h"
#include "controllers/menu_controller.h"
#include "controllers/overworld_controller.h"
#include "entities/entity.h"
#include "entities/entity_manager.h"
#include "entities/player.h"
#include "level.h"

#include <engine/input/input.h>
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
  InputState *input_state;
  lua_State *lua;
  OverworldController *overworld_controller;
  DungeonController *dungeon_controller;
  BattleController *battle_controller;
  MenuController *menu_controller;
  GameStage stage;
  f32 scale;

public:
  Game() {
    this->input_state          = nullptr;
    this->overworld_controller = nullptr;
    this->dungeon_controller   = nullptr;
    this->battle_controller    = nullptr;
    this->menu_controller      = nullptr;
    this->scale                = RENDER_SCALE;
    this->stage                = GameStage::GS_UNKNOWN;
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

  void set_stage(GameState *state, const GameStage stage);
  std::shared_ptr<Player> get_player();
  std::shared_ptr<Entity> get_player_ent();
};

#endif
