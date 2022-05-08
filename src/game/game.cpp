#include "game.h"

#include "binds.h"

#include <cstdlib>
#include <iostream>

static inline const char *pick_asteroid() {
  switch (rand() % 4) {
  case 1: return "assets/asteroid1.png";
  case 2: return "assets/asteroid2.png";
  case 3: return "assets/asteroid3.png";
  default: return "assets/asteroid.png";
  }
}

void Game::init(GameState *state) {
  srand(platform_time_sec());

  this->player = this->ent_create();
  this->player->set_texture_path("assets/ship.png");
  this->player->set_pos({ 0.0f, -0.4f, 0.0f });
  this->player->set_active(true);
}

void Game::update(GameState *state, mutex_t *lock) {
  const f64 update_time = platform_time_f64();

  if (this->input_state != nullptr && this->input_state->command_count > 0) {
    this->process_input(state, update_time);
    input_flush(this->input_state);
  }

  for (std::shared_ptr<Entity> ent : this->entities)
    if (ent->is_active())
      ent->update(update_time);

  const Vector3 player_pos = this->player->get_pos();

  this->player->set_pos({ std::clamp(player_pos.x, -0.95f, 0.95f), std::clamp(player_pos.y, -0.95f, 0.10f), 1.0f });
}

void Game::update_lazy(GameState *state, mutex_t *lock) {
  for (const std::shared_ptr<Entity> ent : this->entities)
    if (ent->should_be_removed())
      this->dangling_renderables.push_back(ent->get_renderable_id());

  std::erase_if(this->entities, [](const std::shared_ptr<Entity> ent) {
    return ent->should_be_removed();
  });

  this->spawn_asteroid();
}

void Game::update_paused(GameState *state, mutex_t *lock) {}

void Game::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {
  for (const u32 id : this->dangling_renderables) {
    renderable_free(render_state, &renderables[id]);
    renderable_make_default(&renderables[id]);
  }

  this->dangling_renderables.clear();

  for (std::shared_ptr<Entity> ent : this->entities) {
    Renderable *r = &renderables[ent->get_renderable_id()];

    if (r == nullptr)
      continue;

    if (ent->is_active()) {
      if (!ent->is_ready()) {
        const u32 renderable_id = this->ent_assign_renderable(renderables, renderables_count, ent);

        ent->set_renderable_id(renderable_id);
        ent->set_ready(true);

        r = &renderables[renderable_id];
      }

      if (!r->active) {
        renderable_init_rect(render_state, r, 0.1f, 0.1f);
        renderable_set_scale(r, ent->get_scale());
        renderable_load_texture(render_state, r, ent->get_texture_path().c_str());
        renderable_set_active(r, GS_TRUE);
      }

      if (r->assigned_to != ent->get_id())
        throw std::runtime_error("Renderable is not assigned to the correct entity.");

      renderable_set_pos(r, vector3_to_vector4(ent->get_pos()));
      renderable_set_rotation(r, ent->get_axis(), -ent->get_angle() + util_radians(90));
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
      case Cmd::LEFT: this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 5.0f); break;
      case -Cmd::LEFT:
        if (this->player->get_angular_velocity() > 0.0f)
          this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 0.0f);
        break;
      case Cmd::RIGHT: this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), -5.0f); break;
      case -Cmd::RIGHT:
        if (this->player->get_angular_velocity() < 0.0f)
          this->player->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), 0.0f);
        break;
      case Cmd::FIRE: this->spawn_projectile(); break;
      default: break;
      }
    }
  }
}

std::shared_ptr<Entity> Game::ent_create(std::shared_ptr<Entity> parent) {
  std::shared_ptr<Entity> ent = std::make_shared<Entity>(this->entities.size());
  ent->set_parent(parent);

  this->entities.push_back(ent);

  return ent;
}

const u32 Game::ent_assign_renderable(
  Renderable *renderables, const u32 renderables_count, const std::shared_ptr<Entity> ent
) const {
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
}

void Game::spawn_projectile() {
  const f64 current_time = platform_time_f64();

  if (this->last_projectile_at + 0.5 <= current_time) {
    std::shared_ptr<Entity> projectile = this->ent_create();

    projectile->set_texture_path("assets/beam.png");
    projectile->set_pos(this->player->get_pos());
    projectile->set_velocity_x(3.0f);
    projectile->rotate(this->player->get_axis(), this->player->get_angle());
    projectile->set_lifetime(2.0);
    projectile->set_active(true);

    this->last_projectile_at = current_time;
  }
}

void Game::spawn_asteroid() {
  const f64 current_time = platform_time_f64();

  if (this->last_asteroid_at + 1.5 <= current_time) {
    std::shared_ptr<Entity> asteroid = this->ent_create();

    const f32 size = 1.0f + rand() % 15 / 10.0f;
    Vector3 pos    = { -1.2f, -1.2f, 0.0f };

    if (rand() % 2 == 0) {
      pos.y = rand() % 2 == 0 ? -1.2f : 0.4f;
      pos.x += rand() % 200 / 100.0f;
    } else {
      pos.y += rand() % 115 / 100.0f;
      pos.x = rand() % 2 == 0 ? -1.2f : 1.2f;
    }

    asteroid->set_texture_path(pick_asteroid());
    asteroid->set_pos(pos);
    asteroid->set_velocity(
      vector_scale3(vector_normal3(vector_sub3(vector_make3(0.0f, -0.4f, 0.0f), pos)), 0.1f + rand() % 4 / 10.0f)
    );
    asteroid->rotate_continuous(vector_make3(0.0f, 0.0f, 1.0f), -0.1 + rand() % 100 / 500.0f);
    asteroid->set_scale({ size, size });
    asteroid->set_lifetime(30.0);
    asteroid->set_active(true);

    this->last_asteroid_at = current_time;
  }
}
