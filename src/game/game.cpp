#include "game.h"

#include "binds.h"
#include "vector.cpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

static const Vector3 center_vec3 = { 0.0f, -0.4375f, 0.0f };

void Game::init(GameState *state) {
  srand(platform_time_sec());

  this->lua = luaL_newstate();
  luaL_openlibs(this->lua);

  this->player = this->ent_create();
  this->player->set_entity_class(EntClass::PLAYER);
  this->player->set_texture_path("assets/debug/player_16x16.png");
  this->player->set_pos(center_vec3);
  this->player->set_active(true);
}

void Game::update(GameState *state, mutex_t *lock) {
  const f64 update_time = platform_time_f64();

  if (this->input_state != nullptr && this->input_state->command_count > 0) {
    this->process_input(state, update_time);
    input_flush(this->input_state);
  }

  for (std::shared_ptr<Entity> ent : this->entities) {
    if (this->is_valid(ent)) {
      ent->update(update_time);
      this->check_collision(ent);
    }
  }

  const Vector3 player_pos = this->player->get_pos();

  this->player->set_pos({ std::clamp(player_pos.x, -0.95f, 0.95f), std::clamp(player_pos.y, -0.95f, 0.10f), 1.0f });
}

void Game::update_lazy(GameState *state, mutex_t *lock) {
  for (std::shared_ptr<Entity> ent : this->entities)
    if (ent->should_be_removed())
      this->dangling_renderables.push_back(ent->get_renderable_id());

  std::erase_if(this->entities, [](std::shared_ptr<Entity> ent) {
    return ent.get() == nullptr || ent->should_be_removed();
  });
}

void Game::update_paused(GameState *state, mutex_t *lock) {}

void Game::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {
  for (const u32 id : this->dangling_renderables) {
    if (!this->can_delete_renderable(id))
      continue;

    if (renderables[id].vertices != NULL && renderables[id].uv != NULL)
      renderable_free(&renderables[id]);

    renderable_set_active(&renderables[id], GS_FALSE);
    renderable_make_default(&renderables[id]);
  }

  this->dangling_renderables.clear();

  for (std::shared_ptr<Entity> ent : this->entities) {
    /* remove the following 2 lines for a free segfault, "smart" pointers aint so "smart" eh */
    if (ent.get() == nullptr)
      continue;

    Renderable *r = &renderables[ent->get_renderable_id()];

    if (r == nullptr)
      continue;

    if (ent->is_active() && !ent->should_be_removed()) {
      if (!ent->is_ready()) {
        const u32 renderable_id = this->ent_assign_renderable(renderables, renderables_count, ent);

        ent->set_renderable_id(renderable_id);
        ent->set_ready(true);

        r = &renderables[renderable_id];
      }

      if (!r->active && this->is_valid(ent)) {
        renderable_init_rect(render_state, r, 0.1f, 0.1f);
        renderable_set_scale(r, ent->get_scale());
        renderable_load_texture(render_state, r, ent->get_texture_path().c_str());
        renderable_set_active(r, GS_TRUE);
      }

      if (r->assigned_to != ent->get_id() && r->assigned_to != -1)
        throw std::runtime_error("Renderable is not assigned to the correct entity.");

      renderable_set_pos(r, vector3_to_vector4(ent->get_pos()));
      renderable_set_rotation(r, ent->get_axis(), -ent->get_angle());
      renderable_set_velocity(r, ent->get_velocity_rotated());
      renderable_set_updated(r, ent->get_updated_at());
    } else if (r->active == GS_TRUE) {
      renderable_set_active(r, GS_FALSE);
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
}

void Game::process_input(GameState *state, const f64 update_time) {
  for (u32 i = 0; i < this->input_state->command_count; i++) {
    if (this->player != nullptr) {
      switch (this->input_state->commands[i]) {
      case Cmd::FORWARD: this->player->set_velocity_y(-0.5f); break;
      case -Cmd::FORWARD:
        if (this->player->get_velocity().y < 0.0f)
          this->player->set_velocity_y(0.0f);
        break;
      case Cmd::BACK: this->player->set_velocity_y(0.5f); break;
      case -Cmd::BACK:
        if (this->player->get_velocity().y > 0.0f)
          this->player->set_velocity_y(0.0f);
        break;
      case Cmd::LEFT: this->player->set_velocity_x(-0.5f); break;
      case -Cmd::LEFT:
        if (this->player->get_velocity().x < 0.0f)
          this->player->set_velocity_x(0.0f);
        break;
      case Cmd::RIGHT: this->player->set_velocity_x(0.5f); break;
      case -Cmd::RIGHT:
        if (this->player->get_velocity().x > 0.0f)
          this->player->set_velocity_x(0.0f);
        break;

      default: break;
      }
    }
  }
}

std::shared_ptr<Entity> Game::ent_create(std::shared_ptr<Entity> parent) {
  std::shared_ptr<Entity> ent = std::make_shared<Entity>((u32)this->entities.size());
  ent->set_parent(parent);

  this->entities.push_back(ent);

  return ent;
}

const u32
  Game::ent_assign_renderable(Renderable *renderables, const u32 renderables_count, std::shared_ptr<Entity> ent) const {
  for (u32 i = 0; i < renderables_count; i++) {
    if (renderables[i].assigned_to == -1) {
      renderable_set_assigned(&renderables[i], ent->get_id());
      return i;
    }
  }

  throw std::runtime_error("No more free renderables.");
}

void Game::ent_remove(std::shared_ptr<Entity> ent) {
  this->dangling_renderables.push_back(ent->get_renderable_id());

  ent->set_should_remove(true);
  ent->set_active(false);
}

void Game::check_collision(std::shared_ptr<Entity> ent) {
  if (!this->is_valid(ent))
    return;

  for (std::shared_ptr<Entity> target : this->entities) {
    if (!this->is_valid(target))
      continue;

    if (target->get_id() == ent->get_id())
      continue;

    if (ent->collides_with(target)) {
      // TODO: collision logic
    }
  }
}

void Game::clear_entities() {
  for (std::shared_ptr<Entity> target : this->entities) {
    if (!this->is_valid(target))
      continue;

    if (target->get_entity_class() != EntClass::PLAYER)
      this->ent_remove(target);
  }
}

bool Game::can_delete_renderable(const u32 renderable_id) {
  for (std::shared_ptr<Entity> ent : this->entities)
    if (this->is_valid(ent) && ent->get_renderable_id() == renderable_id)
      return false;

  return true;
}

bool Game::is_valid(std::shared_ptr<Entity> ent) const { return ent.get() != nullptr && ent->is_valid(); }
