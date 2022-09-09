#include "overworld_controller.h"

#include "../binds.h"
#include "../vector.h"

#define PLAYER_ENT this->base.ent_manager->get_player_ent()

void OverworldController::init(GameState *state) {
  this->level = new Level(this->base.lua, this->base.ent_manager);
  this->level->load_json("levels/dev.json");
  this->level->init();

  PLAYER_ENT->set_pos({ 0.2f, 0.2f, 0.0f });
}

void OverworldController::destroy(GameState *state) {
  this->base.ent_manager->clear_entities();
  delete this->level;
}

void OverworldController::update(GameState *state, mutex_t *lock) {
  for (Entity &ent : this->base.ent_manager->entities)
    if (ent.is_valid() && ent.get_should_collide())
      this->check_collision(&ent);

  this->base.ent_manager->get_player()->update_anim();
}

void OverworldController::update_lazy(GameState *state, mutex_t *lock) {}

void OverworldController::update_paused(GameState *state, mutex_t *lock) {}

void OverworldController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
) {
  if (this->base.ent_manager->player == nullptr)
    return;

  const Entity *ply = this->base.ent_manager->player->get_base();

  if (ply->is_valid()) {
    const f32 delta = (f32)(platform_time_f64() - ply->get_updated_at());

#ifndef _WIN32
    const f32 lerp_factor = 0.05f;
#else
    const f32 lerp_factor = 0.1f;
#endif

    render_state->camera_transform.w[0] = util_lerp_f32(
      lerp_factor,
      render_state->camera_transform.w[0],
      -util_lerp_f32(delta, ply->get_pos().x, ply->get_pos().x + ply->get_velocity().x) * render_state->render_scale
    );

    render_state->camera_transform.w[1] = util_lerp_f32(
      lerp_factor,
      render_state->camera_transform.w[1],
      -util_lerp_f32(delta, ply->get_pos().y, ply->get_pos().y + ply->get_velocity().y) * (1280.0f / 720.0f) *
        render_state->render_scale
    );
  }
}

void OverworldController::process_input(GameState *state, const f64 update_time) {
  for (u32 i = 0; i < this->base.input_state->command_count; i++) {
    if (PLAYER_ENT != nullptr) {
      switch (this->base.input_state->commands[i]) {
      case Cmd::FORWARD: PLAYER_ENT->set_velocity_y(-0.5f); break;
      case -Cmd::FORWARD:
        if (PLAYER_ENT->get_velocity().y < 0.0f)
          PLAYER_ENT->set_velocity_y(0.0f);
        break;
      case Cmd::BACK: PLAYER_ENT->set_velocity_y(0.5f); break;
      case -Cmd::BACK:
        if (PLAYER_ENT->get_velocity().y > 0.0f)
          PLAYER_ENT->set_velocity_y(0.0f);
        break;
      case Cmd::LEFT: PLAYER_ENT->set_velocity_x(-0.5f); break;
      case -Cmd::LEFT:
        if (PLAYER_ENT->get_velocity().x < 0.0f)
          PLAYER_ENT->set_velocity_x(0.0f);
        break;
      case Cmd::RIGHT: PLAYER_ENT->set_velocity_x(0.5f); break;
      case -Cmd::RIGHT:
        if (PLAYER_ENT->get_velocity().x > 0.0f)
          PLAYER_ENT->set_velocity_x(0.0f);
        break;

      default: break;
      }
    }
  }
}

void OverworldController::check_collision(Entity *ent) {
  if (!ent->is_valid())
    return;

  for (Entity &target : this->base.ent_manager->entities) {
    if (!target.is_valid())
      continue;

    if (target.get_id() == ent->get_id())
      continue;

    if (ent->collides_with(&target)) {
      // TODO: collision logic
    }
  }
}
