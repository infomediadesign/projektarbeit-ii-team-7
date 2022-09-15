#include "battle_controller.h"

#include <math.h>

static const Vector3 center_vec3  = { 0.0f, -0.4375f, 0.0f };
static const Vector3 opponent_pos = { 0.0f, -0.4375f - 0.2f, 0.0f };

void BattleController::init(GameState *state) {
  this->base.ent_manager->get_player_ent()->set_pos({ -100.0f, -100.0f });

  this->background = this->base.ent_manager->ent_create();
  this->background->set_ent_class(EntClass::BACKGROUND);
  this->background->set_texture_path("assets/debug/battle_background.png");
  this->background->set_pos(center_vec3);
  this->background->set_scale({ 20.0f, 11.25f });
  this->background->set_active(true);

  LUA_EVENT_RUN(this->base.lua, "battle_setup_opponents");
  LUA_EVENT_CALL(this->base.lua, 0, 1);

  if (lua_isboolean(this->base.lua, -1) && lua_toboolean(this->base.lua, -1) == 0)
    return;

  LUA_EVENT_RUN(this->base.lua, "battle_setup_gui");
  LUA_EVENT_CALL(this->base.lua, 0, 0);
}

void BattleController::update(GameState *state, mutex_t *lock) {
  LUA_EVENT_RUN(this->base.lua, "battle_update");
  LUA_EVENT_CALL(this->base.lua, 0, 0);
}

void BattleController::update_lazy(GameState *state, mutex_t *lock) {}

void BattleController::update_paused(GameState *state, mutex_t *lock) {}

void BattleController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
) {
  render_state->camera_transform.w[0] = 0.0f;
  render_state->camera_transform.w[1] = RENDER_CENTER_OFFSET;
}

void BattleController::destroy(GameState *state) { this->base.ent_manager->clear_entities(); }

void BattleController::process_input(GameState *state, const f64 update_time) {}
