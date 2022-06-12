#include "game.h"

#include "binds.h"
#include "vector.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#define RUN_METHOD(m, args...)                                              \
  switch (this->stage) {                                                    \
  case GameStage::GS_MENU: this->menu_controller->m(args); break;           \
  case GameStage::GS_OVERWORLD: this->overworld_controller->m(args); break; \
  case GameStage::GS_DUNGEON: this->dungeon_controller->m(args); break;     \
  case GameStage::GS_BATTLE: this->battle_controller->m(args); break;       \
  default: break;                                                           \
  }

static const Vector3 center_vec3 = { 0.0f, -0.4375f, 0.0f };

void Game::init(GameState *state) {
  srand(platform_time_sec());

  this->lua = luaL_newstate();
  luaL_openlibs(this->lua);

  this->set_stage(state, GameStage::GS_OVERWORLD);
  this->ent_manager.create_player();
  this->ent_manager.get_player_ent()->set_pos(center_vec3 + vector_make3(0.0f, 0.0f, 1.0f));
}

void Game::update(GameState *state, mutex_t *lock) {
  const f64 update_time = platform_time_f64();

  if (this->input_state != nullptr && this->input_state->command_count > 0) {
    this->process_input(state, update_time);
    input_flush(this->input_state);
  }

  for (std::shared_ptr<Entity> ent : this->ent_manager.entities) {
    if (this->ent_manager.is_valid(ent)) {
      ent->update(update_time);
      this->check_collision(ent);
    }
  }

  RUN_METHOD(update, state, lock)
}

void Game::update_lazy(GameState *state, mutex_t *lock) {
  for (std::shared_ptr<Entity> ent : this->ent_manager.entities)
    if (ent->should_be_removed())
      this->ent_manager.dangling_renderables.push_back(ent->get_renderable_id());

  std::erase_if(this->ent_manager.entities, [](std::shared_ptr<Entity> ent) {
    return ent.get() == nullptr || ent->should_be_removed();
  });

  RUN_METHOD(update_lazy, state, lock)
}

void Game::update_paused(GameState *state, mutex_t *lock) { RUN_METHOD(update_paused, state, lock) }

void Game::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {
  render_state->render_scale = this->scale;

  render_state->camera_transform.x[0] = render_state->render_scale;
  render_state->camera_transform.y[1] = 768.0f / 432.0f * render_state->render_scale;

  /* Clean up renderables that are no longer in use */
  for (const u32 id : this->ent_manager.dangling_renderables) {
    if (!this->ent_manager.can_delete_renderable(id))
      continue;

    if (renderables[id].vertices != NULL && renderables[id].uv != NULL)
      renderable_free(&renderables[id]);

    renderable_set_active(&renderables[id], GS_FALSE);
    renderable_make_default(&renderables[id]);
  }

  this->ent_manager.dangling_renderables.clear();

  /* Assign renderables to entities that don't already have them assigned,
     and updates position and attributes of those which are assigned */
  for (std::shared_ptr<Entity> ent : this->ent_manager.entities) {
    /* remove the following 2 lines for a free segfault, "smart" pointers aint so "smart" eh */
    if (ent.get() == nullptr)
      continue;

    Renderable *r = &renderables[ent->get_renderable_id()];

    if (r == nullptr)
      continue;

    if (ent->get_active() && !ent->should_be_removed()) {
      if (!ent->get_ready()) {
        const u32 renderable_id = this->ent_manager.ent_assign_renderable(renderables, renderables_count, ent);

        ent->set_renderable_id(renderable_id);
        ent->set_ready(true);

        r = &renderables[renderable_id];
      }

      if (!r->active && this->ent_manager.is_valid(ent)) {
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

  RUN_METHOD(update_renderables, state, lock, render_state, renderables, renderables_count)
}

void Game::create_bindings(GameState *state, mutex_t *lock, InputState *input_state) {
  this->input_state = input_state;

  switch (this->stage) {
  case GameStage::GS_MENU: this->menu_controller->get_base()->input_state = this->input_state; break;
  case GameStage::GS_OVERWORLD: this->overworld_controller->get_base()->input_state = this->input_state; break;
  case GameStage::GS_DUNGEON: this->dungeon_controller->get_base()->input_state = this->input_state; break;
  case GameStage::GS_BATTLE: this->battle_controller->get_base()->input_state = this->input_state; break;
  default: break;
  }

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
    switch (this->input_state->commands[i]) {
    case Cmd::ZOOM: this->scale = std::clamp(this->scale + 0.05f, 0.5f, 2.0f); break;
    case -Cmd::ZOOM: this->scale = std::clamp(this->scale - 0.05f, 0.5f, 2.0f); break;
    case Cmd::ZOOM_RESET: this->scale = RENDER_SCALE; break;

    default: break;
    }
  }

  RUN_METHOD(process_input, state, update_time)
}

void Game::check_collision(std::shared_ptr<Entity> ent) {
  if (!this->ent_manager.is_valid(ent))
    return;

  for (std::shared_ptr<Entity> target : this->ent_manager.entities) {
    if (!this->ent_manager.is_valid(target))
      continue;

    if (target->get_id() == ent->get_id())
      continue;

    if (ent->collides_with(target)) {
      // TODO: collision logic
    }
  }
}

void Game::set_stage(GameState *state, const GameStage stage) {
  RUN_METHOD(destroy, state)

  this->stage = stage;

  switch (this->stage) {
  case GameStage::GS_MENU:
    this->menu_controller                          = new MenuController();
    this->menu_controller->get_base()->ent_manager = &this->ent_manager;
    this->menu_controller->get_base()->lua         = this->lua;
    this->menu_controller->get_base()->input_state = this->input_state;
    this->menu_controller->init(state);
    break;
  case GameStage::GS_OVERWORLD:
    this->overworld_controller                          = new OverworldController();
    this->overworld_controller->get_base()->ent_manager = &this->ent_manager;
    this->overworld_controller->get_base()->lua         = this->lua;
    this->overworld_controller->get_base()->input_state = this->input_state;
    this->overworld_controller->init(state);
    break;
  case GameStage::GS_DUNGEON:
    this->dungeon_controller                          = new DungeonController();
    this->dungeon_controller->get_base()->ent_manager = &this->ent_manager;
    this->dungeon_controller->get_base()->lua         = this->lua;
    this->dungeon_controller->get_base()->input_state = this->input_state;
    this->dungeon_controller->init(state);
    break;
  case GameStage::GS_BATTLE:
    this->battle_controller                          = new BattleController();
    this->battle_controller->get_base()->ent_manager = &this->ent_manager;
    this->battle_controller->get_base()->lua         = this->lua;
    this->battle_controller->get_base()->input_state = this->input_state;
    this->battle_controller->init(state);
    break;
  default: break;
  }
}
