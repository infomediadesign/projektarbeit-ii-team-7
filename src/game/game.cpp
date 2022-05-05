#include "game.h"

#include "binds.h"

#include <iostream>

void Game::update(GameState *state, mutex_t *lock) {
  const f64 update_time = platform_time_f64();

  if (this->input_state != nullptr && this->input_state->command_count > 0) {
    this->process_input(state);
    input_flush(this->input_state);
  }

  if (this->player == nullptr) {
    this->player = this->ent_create();
    this->player->set_texture_path("assets/ship.png");
    this->player->set_active(true);
  }

  for (Entity *ent : this->entities)
    if (ent->is_active())
      ent->update(update_time);
}

void Game::update_lazy(GameState *state, mutex_t *lock) {}

void Game::update_paused(GameState *state, mutex_t *lock) {}

void Game::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {
  for (Entity *ent : this->entities) {
    if (ent->is_active()) {
      Renderable *r = &renderables[ent->get_renderable_id()];

      if (r == nullptr)
        continue;

      if (!r->active) {
        renderable_init_rect(render_state, r, 0.1f, 0.1f);
        renderable_load_texture(render_state, r, ent->get_texture_path().c_str());
        renderable_set_active(r, GS_TRUE);
      }

      renderable_set_pos(r, vector3_to_vector4(ent->get_pos()));
      renderable_set_rotation(r, ent->get_axis(), -ent->get_angle() + util_radians(90));
    }
  }
}

void Game::create_bindings(GameState *state, mutex_t *lock, InputState *input_state) {
  this->input_state = input_state;

  input_bind(input_state, MF_KEY_W | MF_KEY_PRESS, Cmd::FORWARD);
  input_bind(input_state, MF_KEY_A | MF_KEY_PRESS, Cmd::LEFT);
  input_bind(input_state, MF_KEY_S | MF_KEY_PRESS, Cmd::BACK);
  input_bind(input_state, MF_KEY_D | MF_KEY_PRESS, Cmd::RIGHT);
  input_bind(input_state, MF_KEY_UP | MF_KEY_PRESS, Cmd::FORWARD);
  input_bind(input_state, MF_KEY_LEFT | MF_KEY_PRESS, Cmd::LEFT);
  input_bind(input_state, MF_KEY_DOWN | MF_KEY_PRESS, Cmd::BACK);
  input_bind(input_state, MF_KEY_RIGHT | MF_KEY_PRESS, Cmd::RIGHT);
  input_bind(input_state, MF_KEY_W | MF_KEY_RELEASE, -Cmd::FORWARD);
  input_bind(input_state, MF_KEY_A | MF_KEY_RELEASE, -Cmd::LEFT);
  input_bind(input_state, MF_KEY_S | MF_KEY_RELEASE, -Cmd::BACK);
  input_bind(input_state, MF_KEY_D | MF_KEY_RELEASE, -Cmd::RIGHT);
  input_bind(input_state, MF_KEY_UP | MF_KEY_RELEASE, -Cmd::FORWARD);
  input_bind(input_state, MF_KEY_LEFT | MF_KEY_RELEASE, -Cmd::LEFT);
  input_bind(input_state, MF_KEY_DOWN | MF_KEY_RELEASE, -Cmd::BACK);
  input_bind(input_state, MF_KEY_RIGHT | MF_KEY_RELEASE, -Cmd::RIGHT);
  input_bind(input_state, MF_KEY_SPACE | MF_KEY_PRESS, Cmd::FIRE);
  input_bind(input_state, MF_KEY_ENTER | MF_KEY_PRESS, Cmd::FIRE);
}

void Game::process_input(GameState *state) {
  for (u32 i = 0; i < this->input_state->command_count; i++) {
    if (this->player != nullptr) {
      switch (this->input_state->commands[i]) {
      case Cmd::FORWARD: this->player->set_velocity_x(0.75f); break;
      case Cmd::BACK: this->player->set_velocity_x(-0.75f); break;
      case -Cmd::FORWARD:
      case -Cmd::BACK: this->player->set_velocity_x(0.0f); break;
      case Cmd::LEFT: this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 0.05f); break;
      case Cmd::RIGHT: this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), -0.05f); break;
      case -Cmd::LEFT:
      case -Cmd::RIGHT: this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 0.0f); break;
      default: break;
      }
    }
  }
}

Entity *Game::ent_create(Entity *parent) {
  Entity *ent = new Entity(this->entities.size());
  ent->set_parent(parent);

  this->entities.push_back(ent);

  return ent;
}
