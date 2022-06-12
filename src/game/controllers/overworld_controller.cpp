#include "overworld_controller.h"

#include "../binds.h"
#include "../vector.h"

static const Vector3 center_vec3 = { 0.0f, -0.4375f, 0.0f };

void OverworldController::init(GameState *state) {
  for (u8 x = 0; x < 16; x++) {
    for (u8 y = 0; y < 16; y++) {
      std::shared_ptr<Entity> ent = this->base.ent_manager->ent_create();
      ent->set_ent_class(EntClass::BACKGROUND);
      ent->set_texture_path(
        (x != 0 && x != 15 && y != 0 && y != 15) ? "assets/debug/path_32x32.png" : "assets/debug/wall_32x32.png"
      );
      ent->set_pos(center_vec3 + vector_make3(-0.8f + 0.1f * x, -0.8f + 0.1f * y, 0.0f));
      ent->set_active(true);
    }
  }
}

void OverworldController::update(GameState *state, mutex_t *lock) {}

void OverworldController::update_lazy(GameState *state, mutex_t *lock) {}

void OverworldController::update_paused(GameState *state, mutex_t *lock) {}

void OverworldController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {
  if (this->base.ent_manager->player == nullptr)
    return;

  const std::shared_ptr<Entity> ply = this->base.ent_manager->player->get_base();

  if (this->base.ent_manager->is_valid(ply)) {
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
      -util_lerp_f32(delta, ply->get_pos().y, ply->get_pos().y + ply->get_velocity().y) * (768.0f / 432.0f) *
        render_state->render_scale
    );
  }
}

void OverworldController::destroy(GameState *state) {}

void OverworldController::process_input(GameState *state, const f64 update_time) {
  for (u32 i = 0; i < this->base.input_state->command_count; i++) {
    if (this->base.ent_manager->get_player_ent() != nullptr) {
      switch (this->base.input_state->commands[i]) {
      case Cmd::FORWARD: this->base.ent_manager->get_player_ent()->set_velocity_y(-0.5f); break;
      case -Cmd::FORWARD:
        if (this->base.ent_manager->get_player_ent()->get_velocity().y < 0.0f)
          this->base.ent_manager->get_player_ent()->set_velocity_y(0.0f);
        break;
      case Cmd::BACK: this->base.ent_manager->get_player_ent()->set_velocity_y(0.5f); break;
      case -Cmd::BACK:
        if (this->base.ent_manager->get_player_ent()->get_velocity().y > 0.0f)
          this->base.ent_manager->get_player_ent()->set_velocity_y(0.0f);
        break;
      case Cmd::LEFT: this->base.ent_manager->get_player_ent()->set_velocity_x(-0.5f); break;
      case -Cmd::LEFT:
        if (this->base.ent_manager->get_player_ent()->get_velocity().x < 0.0f)
          this->base.ent_manager->get_player_ent()->set_velocity_x(0.0f);
        break;
      case Cmd::RIGHT: this->base.ent_manager->get_player_ent()->set_velocity_x(0.5f); break;
      case -Cmd::RIGHT:
        if (this->base.ent_manager->get_player_ent()->get_velocity().x > 0.0f)
          this->base.ent_manager->get_player_ent()->set_velocity_x(0.0f);
        break;

      default: break;
      }
    }
  }
}
