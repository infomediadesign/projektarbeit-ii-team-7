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

  for (u8 x = 0; x < 16; x++) {
    for (u8 y = 0; y < 16; y++) {
      std::shared_ptr<Entity> ent = this->ent_create();
      ent->set_ent_class(EntClass::BACKGROUND);
      ent->set_texture_path("assets/debug/path_32x32.png");
      ent->set_pos(center_vec3 + vector_make3(-0.8f + 0.1f * x, -0.8f + 0.1f * y, 0.0f));
      ent->set_active(true);
    }
  }

  this->player = this->ent_create();
  this->player->set_ent_class(EntClass::PLAYER);
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
  render_state->render_scale = this->scale;

  render_state->camera_transform.x[0] = render_state->render_scale;
  render_state->camera_transform.y[1] = 768.0f / 432.0f * render_state->render_scale;

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

    if (ent->get_active() && !ent->should_be_removed()) {
      if (!ent->get_ready()) {
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

  if (this->is_valid(this->player)) {
    const f32 delta       = (f32)(platform_time_f64() - this->player->get_updated_at());
    const f32 lerp_factor = 0.05f;

    render_state->camera_transform.w[0] = util_lerp_f32(
      lerp_factor,
      render_state->camera_transform.w[0],
      -util_lerp_f32(delta, this->player->get_pos().x, this->player->get_pos().x + this->player->get_velocity().x) *
        render_state->render_scale
    );

    render_state->camera_transform.w[1] = util_lerp_f32(
      lerp_factor,
      render_state->camera_transform.w[1],
      -util_lerp_f32(delta, this->player->get_pos().y, this->player->get_pos().y + this->player->get_velocity().y) *
        (768.0f / 432.0f) * render_state->render_scale
    );
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

  input_bind(input_state, MF_KEY_F1 | MF_KEY_PRESS, Cmd::ZOOM);
  input_bind(input_state, MF_KEY_F2 | MF_KEY_PRESS, -Cmd::ZOOM);
  input_bind(input_state, MF_KEY_F3 | MF_KEY_PRESS, Cmd::ZOOM_RESET);
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
      case Cmd::ZOOM: this->scale = std::clamp(this->scale + 0.05f, 0.5f, 2.0f); break;
      case -Cmd::ZOOM: this->scale = std::clamp(this->scale - 0.05f, 0.5f, 2.0f); break;
      case Cmd::ZOOM_RESET: this->scale = RENDER_SCALE; break;

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

    if (target->get_ent_class() != EntClass::PLAYER)
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
