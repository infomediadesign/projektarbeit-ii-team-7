#include "game.h"

#include "binds.h"

#include <iostream>

void Game::init(GameState *state) {
  this->player = this->ent_create();
  this->player->set_texture_path("assets/ship.png");
  this->player->set_active(true);
}

void Game::update(GameState *state, mutex_t *lock) {
  const f64 update_time = platform_time_f64();

  if (this->input_state != nullptr && this->input_state->command_count > 0) {
    this->process_input(state, update_time);
    input_flush(this->input_state);
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
  for (const u32 id : this->dangling_renderables) {
    renderable_free(render_state, &renderables[id]);
    renderable_make_default(&renderables[id]);
  }

  for (Entity *ent : this->entities) {
    if (ent->is_active()) {
      Renderable *r = &renderables[ent->get_renderable_id()];

      if (r == nullptr)
        continue;

      if (!ent->is_ready()) {
        const u32 renderable_id = this->ent_assign_renderable(renderables, renderables_count, ent);

        ent->set_renderable_id(renderable_id);
        ent->set_ready(true);

        r = &renderables[renderable_id];
      }

      if (!r->active) {
        renderable_init_rect(render_state, r, 0.1f, 0.1f);
        renderable_load_texture(render_state, r, ent->get_texture_path().c_str());
        renderable_set_active(r, GS_TRUE);
      }

      if (r->assigned_to != ent->get_id())
        throw std::runtime_error("Renderable is not assigned to the correct entity.");

      renderable_set_pos(r, vector3_to_vector4(ent->get_pos()));
      renderable_set_rotation(r, ent->get_axis(), -ent->get_angle() + util_radians(90));
      renderable_set_velocity(r, ent->get_velocity_rotated());
      renderable_set_updated(r, ent->get_updated_at());
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

void Game::process_input(GameState *state, const f64 update_time) {
  for (u32 i = 0; i < this->input_state->command_count; i++) {
    if (this->player != nullptr) {
      switch (this->input_state->commands[i]) {
      case Cmd::FORWARD: this->player->set_velocity_x(0.75f); break;
      case -Cmd::FORWARD:
        if (this->player->get_velocity().x > 0.0f)
          this->player->set_velocity_x(0.0f);
        break;
      case Cmd::BACK: this->player->set_velocity_x(-0.75f); break;
      case -Cmd::BACK:
        if (this->player->get_velocity().x < 0.0f)
          this->player->set_velocity_x(0.0f);
        break;
      case Cmd::LEFT: this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 2.5f); break;
      case -Cmd::LEFT:
        if (this->player->get_angular_velocity() > 0.0f)
          this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 0.0f);
        break;
      case Cmd::RIGHT: this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), -2.5f); break;
      case -Cmd::RIGHT:
        if (this->player->get_angular_velocity() < 0.0f)
          this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 0.0f);
        break;
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

const u32 Game::ent_assign_renderable(Renderable *renderables, const u32 renderables_count, const Entity *ent) const {
  for (u32 i = 0; i < renderables_count; i++) {
    if (renderables[i].assigned_to == -1) {
      renderable_set_assigned(&renderables[i], ent->get_id());
      return i;
    }
  }

  throw std::runtime_error("No more free renderables.");
}

void Game::ent_remove(Entity *ent) {
  this->dangling_renderables.push_back(ent->get_renderable_id());
  this->entities.erase(this->entities.begin() + ent->get_entity_index());
  delete ent;
}
