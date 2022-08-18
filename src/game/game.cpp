#include "game.h"

#include "binds.h"
#include "vector.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#define RUN_METHOD(m, ...)                                                         \
  switch (this->stage) {                                                           \
  case GameStage::GS_MENU: this->menu_controller->m(__VA_ARGS__); break;           \
  case GameStage::GS_OVERWORLD: this->overworld_controller->m(__VA_ARGS__); break; \
  case GameStage::GS_DUNGEON: this->dungeon_controller->m(__VA_ARGS__); break;     \
  case GameStage::GS_BATTLE: this->battle_controller->m(__VA_ARGS__); break;       \
  default: break;                                                                  \
  }

#define STAGE_INIT(n, cn)                                             \
  this->n##_controller                          = new cn();           \
  this->n##_controller->get_base()->ent_manager = &this->ent_manager; \
  this->n##_controller->get_base()->lua         = this->lua;          \
  this->n##_controller->get_base()->input_state = this->input_state;  \
  this->n##_controller->init(state)

static const Vector3 center_vec3 = { 0.0f, -0.4375f, 0.0f };

void Game::init(GameState *state) {
  srand(platform_time_sec());

  this->lua = luaL_newstate();
  luaL_openlibs(this->lua);

  this->ent_manager.create_player();
  this->ent_manager.get_player_ent()->set_pos(center_vec3 + vector_make3(0.0f, 0.0f, 1.0f));
  this->ent_manager.get_player_ent()->set_should_sort(true);
  this->set_stage(state, GameStage::GS_OVERWORLD);
}

void Game::update(GameState *state, mutex_t *lock) {
  const f64 update_time = platform_time_f64();

  if (this->input_state != nullptr && this->input_state->command_count > 0) {
    this->process_input(state, update_time);
    input_flush(this->input_state);
  }

  for (std::shared_ptr<Entity> ent : this->ent_manager.entities)
    if (this->ent_manager.is_valid(ent))
      ent->update(update_time);

  RUN_METHOD(update, state, lock)
}

void Game::update_lazy(GameState *state, mutex_t *lock) {
  this->locked = true;

  for (std::shared_ptr<Entity> ent : this->ent_manager.entities)
    if (ent->should_be_removed())
      this->ent_manager.dangling_renderables.push_back(ent->get_renderable());

  std::erase_if(this->ent_manager.entities, [](std::shared_ptr<Entity> ent) {
    return ent.get() == nullptr || ent->should_be_removed();
  });

  RUN_METHOD(update_lazy, state, lock)

  this->locked = false;
}

void Game::update_paused(GameState *state, mutex_t *lock) { RUN_METHOD(update_paused, state, lock) }

int Game::compare_renderables(const void *v1, const void *v2) {
  const Renderable *r1 = *(Renderable **)v1;
  const Renderable *r2 = *(Renderable **)v2;

  if (r1->active == GS_TRUE && r2->active == GS_FALSE)
    return -1;

  if (r1->should_zsort == GS_TRUE) {
    if (r2->should_zsort == GS_FALSE)
      return 1;

    if (r1->position.y > r2->position.y)
      return -1;
  }

  return 0;
}

void Game::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
) {
  render_state->render_scale = this->scale;

  render_state->camera_transform.x[0] = render_state->render_scale;
  render_state->camera_transform.y[1] = 768.0f / 432.0f * render_state->render_scale;

  /* Clean up renderables that are no longer in use */
  for (Renderable *renderable : this->ent_manager.dangling_renderables) {
    if (!this->ent_manager.can_delete_renderable(renderable))
      continue;

    if (renderable->vertices != NULL && renderable->uv != NULL)
      renderable_free(render_state, renderable);

    renderable_set_active(renderable, GS_FALSE);
    renderable_make_default(renderable);
  }

  this->ent_manager.dangling_renderables.clear();

  /* Z sort */
  qsort(renderables, renderables_count, sizeof(Renderable *), Game::compare_renderables);

  if (this->locked)
    return;

  /* Assign renderables to entities that don't already have them assigned,
     and updates position and attributes of those which are assigned */
  for (std::shared_ptr<Entity> ent : this->ent_manager.entities) {
    /* remove the following 2 lines for a free segfault, "smart" pointers aint so "smart" eh */
    if (ent.get() == nullptr || !ent->get_visible())
      continue;

    Renderable *r = ent->get_renderable();

    if (ent->get_active() && !ent->should_be_removed()) {
      if (!ent->get_ready()) {
        Renderable *const renderable = this->ent_manager.ent_assign_renderable(renderables, renderables_count, ent);

        ent->set_renderable(renderable);
        ent->set_ready(true);

        r = renderable;
      }

      if (r == nullptr)
        continue;

      if (r->active != GS_TRUE && this->ent_manager.is_valid(ent)) {
        const Vector2 uv_size = ent->get_uv_size();

        renderable_init_rect_ex(render_state, r, 0.1f, 0.1f, uv_size.x, uv_size.y);
        renderable_set_scale(r, ent->get_scale());
        renderable_load_texture(render_state, r, ent->get_texture_path().c_str());
        renderable_set_active(r, GS_TRUE);
      }

      if (r->assigned_to != ent->get_id() && r->assigned_to != -1)
        throw std::runtime_error("Renderable is not assigned to the correct entity.");

      renderable_set_pos(r, vector3_to_vector4(ent->get_pos()));
      renderable_set_rotation(r, ent->get_axis(), -ent->get_angle());
      renderable_set_velocity(r, ent->get_velocity());
      renderable_set_should_zsort(r, ent->get_should_sort() ? GS_TRUE : GS_FALSE);
      renderable_set_uv_offset(r, ent->get_uv_offset());
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

  /* Movement */
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

  /* Interactions */
  input_bind(input_state, MF_KEY_E | MF_KEY_PRESS, Cmd::USE);
  input_bind(input_state, MF_KEY_E | MF_KEY_RELEASE, -Cmd::USE);
  input_bind(input_state, MF_KEY_ENTER | MF_KEY_PRESS, Cmd::USE);
  input_bind(input_state, MF_KEY_ENTER | MF_KEY_RELEASE, -Cmd::USE);
  input_bind(input_state, MF_KEY_F6 | MF_KEY_PRESS, Cmd::SAVE);
  input_bind(input_state, MF_KEY_F9 | MF_KEY_PRESS, Cmd::LOAD);

  /* Menus */
  input_bind(input_state, MF_KEY_TAB | MF_KEY_PRESS, Cmd::INVENTORY);
  input_bind(input_state, MF_KEY_TAB | MF_KEY_RELEASE, -Cmd::INVENTORY);
  input_bind(input_state, MF_KEY_I | MF_KEY_PRESS, Cmd::INVENTORY);
  input_bind(input_state, MF_KEY_I | MF_KEY_RELEASE, -Cmd::INVENTORY);
  input_bind(input_state, MF_KEY_RIGHT_CONTROL | MF_KEY_PRESS, Cmd::INVENTORY);
  input_bind(input_state, MF_KEY_RIGHT_CONTROL | MF_KEY_RELEASE, -Cmd::INVENTORY);
  input_bind(input_state, MF_KEY_ESCAPE | MF_KEY_PRESS, Cmd::MENU);
  input_bind(input_state, MF_KEY_ESCAPE | MF_KEY_RELEASE, -Cmd::MENU);
  input_bind(input_state, MF_KEY_BACKSPACE | MF_KEY_PRESS, Cmd::MENU);
  input_bind(input_state, MF_KEY_BACKSPACE | MF_KEY_RELEASE, -Cmd::MENU);

  /* Zoom */
  input_bind(input_state, MF_KEY_F1 | MF_KEY_PRESS, Cmd::ZOOM);
  input_bind(input_state, MF_KEY_F2 | MF_KEY_PRESS, -Cmd::ZOOM);
  input_bind(input_state, MF_KEY_F3 | MF_KEY_PRESS, Cmd::ZOOM_RESET);

  /* Debug */
  if (game_is_debug(state)) {
    input_bind(input_state, MF_KEY_F7 | MF_KEY_PRESS, Cmd::DEBUG_OVERWORLD);
    input_bind(input_state, MF_KEY_F8 | MF_KEY_PRESS, Cmd::DEBUG_BATTLE);
  }
}

void Game::process_input(GameState *state, const f64 update_time) {
  for (u32 i = 0; i < this->input_state->command_count; i++) {
    switch (this->input_state->commands[i]) {
    case Cmd::ZOOM: this->scale = std::clamp(this->scale + 0.05f, 0.5f, 2.0f); break;
    case -Cmd::ZOOM: this->scale = std::clamp(this->scale - 0.05f, 0.5f, 2.0f); break;
    case Cmd::ZOOM_RESET: this->scale = RENDER_SCALE; break;
    case Cmd::DEBUG_OVERWORLD: this->set_stage(state, GameStage::GS_OVERWORLD); break;
    case Cmd::DEBUG_BATTLE: this->set_stage(state, GameStage::GS_BATTLE); break;

    default: break;
    }
  }

  RUN_METHOD(process_input, state, update_time)
}

void Game::set_stage(GameState *state, const GameStage stage) {
  this->locked = true;

  RUN_METHOD(destroy, state)

  this->stage = stage;

  switch (this->stage) {
  case GameStage::GS_MENU: STAGE_INIT(menu, MenuController); break;
  case GameStage::GS_OVERWORLD: STAGE_INIT(overworld, OverworldController); break;
  case GameStage::GS_DUNGEON: STAGE_INIT(dungeon, DungeonController); break;
  case GameStage::GS_BATTLE: STAGE_INIT(battle, BattleController); break;
  default: break;
  }

  this->locked = false;
}
