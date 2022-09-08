#include "game.h"

#include "binds.h"
#include "lua/common.h"
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
  this->n##_controller->init(this->game_state)

#define LUA_GET_GAME(L)                              \
  lua_pushlightuserdata(L, (void *)&lua_game_index); \
  lua_gettable(L, LUA_REGISTRYINDEX);                \
  Game *GAME = (Game *)lua_touserdata(L, -1);

static const Vector3 center_vec3     = { 0.0f, -0.4375f, 0.0f };
static const luaL_Reg lua_game_lib[] = {
  { "setstage", Game::lua_set_stage },
  { "bind", Game::lua_bind },
  { "getstage", Game::lua_get_stage },
  { "player", Game::lua_player },
  { NULL, NULL } /* sentinel */
};
static const luaL_Reg lua_ent_lib[] = {
  { "create", Game::lua_ent_create },
  { "remove", Game::lua_ent_remove },
  { "clear", Game::lua_ent_clear },
  { NULL, NULL } /* sentinel */
};
static const i8 lua_game_index = 0;

void Game::init(GameState *state) {
  this->game_state = state;

  srand(platform_time_sec());

  this->init_lua();

  LUA_EVENT_RUN(this->lua, "init");
  LUA_EVENT_CALL(this->lua, 0, 0);

  this->ent_manager.create_player();
  this->ent_manager.get_player_ent()->set_pos(center_vec3 + vector_make3(0.0f, 0.0f, 1.0f));
  this->ent_manager.get_player_ent()->set_should_sort(true);
  this->set_stage(GameStage::GS_OVERWORLD);

  LUA_EVENT_RUN(this->lua, "post_init");
  LUA_EVENT_CALL(this->lua, 0, 0);
}

void Game::update(GameState *state, mutex_t *lock) {
  this->game_state = state;

  LUA_EVENT_RUN(this->lua, "pre_update");
  LUA_EVENT_CALL(this->lua, 0, 0);

  const f64 update_time = platform_time_f64();

  if (this->input_state != nullptr && this->lua != nullptr) {
    this->process_input(state, update_time);
    input_flush(this->input_state);
  }

  for (Entity &ent : this->ent_manager.entities)
    if (ent.is_valid())
      ent.update(update_time);

  LUA_EVENT_RUN(this->lua, "update");
  LUA_EVENT_CALL(this->lua, 0, 0);

  RUN_METHOD(update, state, lock)

  LUA_EVENT_RUN(this->lua, "post_update");
  LUA_EVENT_CALL(this->lua, 0, 0);
}

void Game::update_lazy(GameState *state, mutex_t *lock) {
  this->game_state = state;

  LUA_EVENT_RUN(this->lua, "pre_update_lazy");
  LUA_EVENT_CALL(this->lua, 0, 0);

  this->locked = true;

  for (Entity &ent : this->ent_manager.entities) {
    if (ent.should_be_removed()) {
      this->ent_manager.dangling_renderables.push_back(ent.get_renderable());
      ent.set_default();
    }
  }

  LUA_EVENT_RUN(this->lua, "update_lazy");
  LUA_EVENT_CALL(this->lua, 0, 0);

  RUN_METHOD(update_lazy, state, lock)

  LUA_EVENT_RUN(this->lua, "post_update_lazy");
  LUA_EVENT_CALL(this->lua, 0, 0);

  this->locked = false;
}

void Game::update_paused(GameState *state, mutex_t *lock) {
  this->game_state = state;

  LUA_EVENT_RUN(this->lua, "pre_update_paused");
  LUA_EVENT_CALL(this->lua, 0, 0);

  LUA_EVENT_RUN(this->lua, "update_paused");
  LUA_EVENT_CALL(this->lua, 0, 0);

  RUN_METHOD(update_paused, state, lock)

  LUA_EVENT_RUN(this->lua, "post_update_paused");
  LUA_EVENT_CALL(this->lua, 0, 0);
}

i32 Game::compare_renderables(const void *v1, const void *v2) {
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
  this->game_state           = state;
  render_state->render_scale = this->scale;

  render_state->camera_transform.x[0] = render_state->render_scale;
  render_state->camera_transform.y[1] = 1280.0f / 720.0f * render_state->render_scale;

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
  for (Entity &ent : this->ent_manager.entities) {
    if (!ent.get_visible())
      continue;

    Renderable *r = ent.get_renderable();

    if (ent.get_active() && !ent.should_be_removed()) {
      if (!ent.get_ready()) {
        Renderable *const renderable = this->ent_manager.ent_assign_renderable(renderables, renderables_count, &ent);

        ent.set_renderable(renderable);
        ent.set_ready(true);

        r = renderable;
      }

      if (r == nullptr)
        continue;

      if (r->active != GS_TRUE && ent.is_valid()) {
        const Vector2 uv_size = ent.get_uv_size();

        renderable_init_rect_ex(render_state, r, 0.1f, 0.1f, uv_size.x, uv_size.y);
        renderable_set_scale(r, ent.get_scale());
        renderable_load_texture(render_state, r, ent.get_texture_path().c_str());
        renderable_set_active(r, GS_TRUE);
      }

      if (r->assigned_to != ent.get_id() && r->assigned_to != -1)
        throw std::runtime_error("Renderable is not assigned to the correct entity.");

      renderable_set_pos(r, vector3_to_vector4(ent.get_pos()));
      renderable_set_rotation(r, ent.get_axis(), -ent.get_angle());
      renderable_set_velocity(r, ent.get_velocity());
      renderable_set_should_zsort(r, ent.get_should_sort() ? GS_TRUE : GS_FALSE);
      renderable_set_uv_offset(r, ent.get_uv_offset());
      renderable_set_scale(r, ent.get_scale());
      renderable_set_updated(r, ent.get_updated_at());
    } else if (r != nullptr && r->active == GS_TRUE) {
      renderable_set_active(r, GS_FALSE);
    }
  }

  RUN_METHOD(update_renderables, state, lock, render_state, renderables, renderables_count)
}

void Game::create_bindings(GameState *state, mutex_t *lock, InputState *input_state) {
  this->game_state  = state;
  this->input_state = input_state;

  switch (this->stage) {
  case GameStage::GS_MENU: this->menu_controller->get_base()->input_state = this->input_state; break;
  case GameStage::GS_OVERWORLD: this->overworld_controller->get_base()->input_state = this->input_state; break;
  case GameStage::GS_DUNGEON: this->dungeon_controller->get_base()->input_state = this->input_state; break;
  case GameStage::GS_BATTLE: this->battle_controller->get_base()->input_state = this->input_state; break;
  default: break;
  }

  LUA_EVENT_RUN(this->lua, "pre_create_bindings");
  LUA_EVENT_CALL(this->lua, 0, 1);

  if (!lua_isboolean(this->lua, -1) || lua_toboolean(this->lua, -1) == 1) {
    /* Movement */
    input_bind(input_state, MF_KEY_W | MF_KEY_PRESS, Cmd::FORWARD);
    input_bind(input_state, MF_KEY_A | MF_KEY_PRESS, Cmd::LEFT);
    input_bind(input_state, MF_KEY_S | MF_KEY_PRESS, Cmd::BACK);
    input_bind(input_state, MF_KEY_D | MF_KEY_PRESS, Cmd::RIGHT);
    input_bind(input_state, MF_KEY_UP | MF_KEY_PRESS, Cmd::FORWARD);
    input_bind(input_state, MF_KEY_LEFT | MF_KEY_PRESS, Cmd::LEFT);
    input_bind(input_state, MF_KEY_DOWN | MF_KEY_PRESS, Cmd::BACK);
    input_bind(input_state, MF_KEY_RIGHT | MF_KEY_PRESS, Cmd::RIGHT);
    input_bind(input_state, MF_GAMEPAD_DPAD_UP | MF_KEY_PRESS, Cmd::FORWARD);
    input_bind(input_state, MF_GAMEPAD_DPAD_LEFT | MF_KEY_PRESS, Cmd::LEFT);
    input_bind(input_state, MF_GAMEPAD_DPAD_DOWN | MF_KEY_PRESS, Cmd::BACK);
    input_bind(input_state, MF_GAMEPAD_DPAD_RIGHT | MF_KEY_PRESS, Cmd::RIGHT);
    input_bind(input_state, MF_KEY_W | MF_KEY_RELEASE, -Cmd::FORWARD);
    input_bind(input_state, MF_KEY_A | MF_KEY_RELEASE, -Cmd::LEFT);
    input_bind(input_state, MF_KEY_S | MF_KEY_RELEASE, -Cmd::BACK);
    input_bind(input_state, MF_KEY_D | MF_KEY_RELEASE, -Cmd::RIGHT);
    input_bind(input_state, MF_KEY_UP | MF_KEY_RELEASE, -Cmd::FORWARD);
    input_bind(input_state, MF_KEY_LEFT | MF_KEY_RELEASE, -Cmd::LEFT);
    input_bind(input_state, MF_KEY_DOWN | MF_KEY_RELEASE, -Cmd::BACK);
    input_bind(input_state, MF_KEY_RIGHT | MF_KEY_RELEASE, -Cmd::RIGHT);
    input_bind(input_state, MF_GAMEPAD_DPAD_UP | MF_KEY_RELEASE, -Cmd::FORWARD);
    input_bind(input_state, MF_GAMEPAD_DPAD_LEFT | MF_KEY_RELEASE, -Cmd::LEFT);
    input_bind(input_state, MF_GAMEPAD_DPAD_DOWN | MF_KEY_RELEASE, -Cmd::BACK);
    input_bind(input_state, MF_GAMEPAD_DPAD_RIGHT | MF_KEY_RELEASE, -Cmd::RIGHT);

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

  LUA_EVENT_RUN(this->lua, "create_bindings");
  LUA_EVENT_CALL(this->lua, 0, 0);

  LUA_EVENT_RUN(this->lua, "post_create_bindings");
  LUA_EVENT_CALL(this->lua, 0, 0);
}

void Game::process_input(GameState *state, const f64 update_time) {
  for (u32 i = 0; i < this->input_state->command_count; i++) {
    switch (this->input_state->commands[i]) {
    case Cmd::ZOOM: this->scale = std::clamp(this->scale + 0.05f, 0.5f, 2.0f); break;
    case -Cmd::ZOOM: this->scale = std::clamp(this->scale - 0.05f, 0.5f, 2.0f); break;
    case Cmd::ZOOM_RESET: this->scale = RENDER_SCALE; break;
    case Cmd::DEBUG_OVERWORLD: this->set_stage(GameStage::GS_OVERWORLD); break;
    case Cmd::DEBUG_BATTLE: this->set_stage(GameStage::GS_BATTLE); break;

    default: break;
    }
  }

  LUA_EVENT_RUN(this->lua, "process_input");
  lua_createtable(this->lua, this->input_state->command_count, 0);

  for (u32 i = 0; i < this->input_state->command_count; i++) {
    /* Useless comment! Otherwise linter removes brackets here, and that errors */
    LUA_TABLE_INSERT(this->lua, number, i + 1, this->input_state->commands[i]);
  }

  lua_createtable(this->lua, 0, 13);

  lua_pushstring(this->lua, "left_stick");
  lua_createtable(this->lua, 0, 2);
  LUA_TABLE_SET_NAMED(this->lua, number, "x", this->input_state->left_stick.x);
  LUA_TABLE_SET_NAMED(this->lua, number, "y", this->input_state->left_stick.y);
  lua_settable(this->lua, -3);

  lua_pushstring(this->lua, "left_stick_last");
  lua_createtable(this->lua, 0, 2);
  LUA_TABLE_SET_NAMED(this->lua, number, "x", this->input_state->left_stick_last.x);
  LUA_TABLE_SET_NAMED(this->lua, number, "y", this->input_state->left_stick_last.y);
  lua_settable(this->lua, -3);

  lua_pushstring(this->lua, "right_stick");
  lua_createtable(this->lua, 0, 2);
  LUA_TABLE_SET_NAMED(this->lua, number, "x", this->input_state->right_stick.x);
  LUA_TABLE_SET_NAMED(this->lua, number, "y", this->input_state->right_stick.y);
  lua_settable(this->lua, -3);

  lua_pushstring(this->lua, "right_stick_last");
  lua_createtable(this->lua, 0, 2);
  LUA_TABLE_SET_NAMED(this->lua, number, "x", this->input_state->right_stick_last.x);
  LUA_TABLE_SET_NAMED(this->lua, number, "y", this->input_state->right_stick_last.y);
  lua_settable(this->lua, -3);

  lua_pushstring(this->lua, "mouse");
  lua_createtable(this->lua, 0, 2);
  LUA_TABLE_SET_NAMED(this->lua, number, "x", this->input_state->mouse.x);
  LUA_TABLE_SET_NAMED(this->lua, number, "y", this->input_state->mouse.y);
  lua_settable(this->lua, -3);

  lua_pushstring(this->lua, "mouse_last");
  lua_createtable(this->lua, 0, 2);
  LUA_TABLE_SET_NAMED(this->lua, number, "x", this->input_state->mouse_last.x);
  LUA_TABLE_SET_NAMED(this->lua, number, "y", this->input_state->mouse_last.y);
  lua_settable(this->lua, -3);

  LUA_TABLE_SET_NAMED(this->lua, number, "left_trigger", this->input_state->left_trigger);
  LUA_TABLE_SET_NAMED(this->lua, number, "left_trigger_last", this->input_state->left_trigger_last);
  LUA_TABLE_SET_NAMED(this->lua, number, "right_trigger", this->input_state->right_trigger);
  LUA_TABLE_SET_NAMED(this->lua, number, "right_trigger_last", this->input_state->right_trigger_last);
  LUA_TABLE_SET_NAMED(this->lua, number, "top_binding", this->input_state->top_binding);
  LUA_TABLE_SET_NAMED(this->lua, number, "command_count", this->input_state->command_count);
  LUA_TABLE_SET_NAMED(this->lua, number, "raw_count", this->input_state->raw_count);

  LUA_EVENT_CALL(this->lua, 2, 1);

  if (lua_isboolean(this->lua, -1) && lua_toboolean(this->lua, -1) == 0)
    return;

  RUN_METHOD(process_input, state, update_time)
}

void Game::set_stage(const GameStage stage) {
  this->locked = true;

  LUA_EVENT_RUN(this->lua, "pre_stage_destroy");
  LUA_EVENT_CALL(this->lua, 0, 1);

  if (lua_isboolean(this->lua, -1) && lua_toboolean(this->lua, -1) == 0) {
    this->locked = false;
    return;
  }

  RUN_METHOD(destroy, this->game_state)

  LUA_EVENT_RUN(this->lua, "post_stage_destroy");
  LUA_EVENT_CALL(this->lua, 0, 0);

  LUA_EVENT_RUN(this->lua, "pre_set_stage");
  lua_pushnumber(this->lua, this->stage);
  lua_pushnumber(this->lua, stage);
  LUA_EVENT_CALL(this->lua, 2, 1);

  if (lua_isboolean(this->lua, -1) && lua_toboolean(this->lua, -1) == 0) {
    this->locked = false;
    return;
  }

  this->stage = stage;

  switch (this->stage) {
  case GameStage::GS_MENU: STAGE_INIT(menu, MenuController); break;
  case GameStage::GS_OVERWORLD: STAGE_INIT(overworld, OverworldController); break;
  case GameStage::GS_DUNGEON: STAGE_INIT(dungeon, DungeonController); break;
  case GameStage::GS_BATTLE: STAGE_INIT(battle, BattleController); break;
  default: break;
  }

  lua_pushnumber(this->lua, this->stage);
  lua_setglobal(this->lua, "GAME_STAGE");

  LUA_EVENT_RUN(this->lua, "post_set_stage");
  LUA_EVENT_CALL(this->lua, 0, 0);

  this->locked = false;
}

void Game::ent_remove(Entity *ent) { this->ent_manager.ent_remove(ent); }

void Game::clear_entities() { this->ent_manager.clear_entities(); }

void Game::init_lua() {
  this->lua = luaL_newstate();
  luaL_openlibs(this->lua);

  lua_pushlightuserdata(this->lua, (void *)&lua_game_index);
  lua_pushlightuserdata(this->lua, (void *)this);
  lua_settable(this->lua, LUA_REGISTRYINDEX);

  luaL_register(this->lua, "game", lua_game_lib);
  luaL_register(this->lua, "ent", lua_ent_lib);

  lua_register_common(this->lua);

  char real_path[256];

  asset_find("lua/init.lua", real_path);

  lua_pushstring(this->lua, real_path);
  lua_setglobal(this->lua, "INIT_FILE");

  lua_newtable(this->lua);
  lua_setglobal(this->lua, "ENTS");

  lua_newtable(this->lua);
  lua_setglobal(this->lua, "GAME");

  if (luaL_dofile(this->lua, real_path)) {
    std::cout << "Lua error:" << std::endl;

    if (lua_isstring(this->lua, -1)) {
      const char *err = luaL_checkstring(this->lua, -1);

      std::cout << err << std::endl;
    } else {
      std::cout << "(error not on stack)" << std::endl;
    }
  }

  this->ent_manager.lua = this->lua;
}

i32 Game::lua_set_stage(lua_State *state) {
  f64 idx = luaL_checknumber(state, 1);

  LUA_GET_GAME(state);

  GAME->set_stage((GameStage)idx);

  if (idx >= 0.0 && idx <= (f64)GameStage::GS_UNKNOWN)
    lua_pushboolean(state, 1);
  else
    lua_pushboolean(state, 0);

  return 1;
}

i32 Game::lua_bind(lua_State *state) {
  f64 key = luaL_checknumber(state, 1);
  f64 cmd = luaL_checknumber(state, 2);

  LUA_GET_GAME(state);

  input_bind(GAME->get_input_state(), key, cmd);

  return 0;
}

i32 Game::lua_get_stage(lua_State *state) {
  LUA_GET_GAME(state);

  lua_pushnumber(state, GAME->get_stage());

  return 1;
}

i32 Game::lua_ent_create(lua_State *state) {
  LUA_GET_GAME(state);

  Entity *ent = GAME->ent_manager.ent_create();

  if (ent->get_entity_index() > 0)
    lua_push_entity(state, ent);
  else
    lua_pushnil(state);

  return 1;
}

i32 Game::lua_player(lua_State *state) {
  LUA_GET_GAME(state);

  Entity *ent = GAME->ent_manager.get_player_ent();

  if (ent != nullptr && ent->get_entity_index() > 0)
    lua_push_entity(state, ent);
  else
    lua_pushnil(state);

  return 1;
}

i32 Game::lua_ent_clear(lua_State *state) {
  LUA_GET_GAME(state);

  GAME->clear_entities();

  return 0;
}

i32 Game::lua_ent_remove(lua_State *state) {
  LUA_GET_GAME(state);

  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  GAME->ent_remove(*ent_ptr);

  return 0;
}
