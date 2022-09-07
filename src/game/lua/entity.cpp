#include "entity.h"

#define ENT_LUA_GETTER(n, t)                                                       \
  i32 lua_entity_get_##n(lua_State *L) {                                           \
    const Entity **ent_ptr = (const Entity **)luaL_checkudata(L, 1, "EntityMeta"); \
    lua_push##t(L, (*ent_ptr)->get_##n());                                         \
    return 1;                                                                      \
  }

#define ENT_LUA_STRING_GETTER(n)                                                   \
  i32 lua_entity_get_##n(lua_State *L) {                                           \
    const Entity **ent_ptr = (const Entity **)luaL_checkudata(L, 1, "EntityMeta"); \
    lua_pushstring(L, (*ent_ptr)->get_##n().c_str());                              \
    return 1;                                                                      \
  }

#define ENT_LUA_VEC2_GETTER(n)                                                     \
  i32 lua_entity_get_##n(lua_State *L) {                                           \
    const Entity **ent_ptr = (const Entity **)luaL_checkudata(L, 1, "EntityMeta"); \
    const Vector2 v        = (*ent_ptr)->get_##n();                                \
    lua_createtable(L, 2, 0);                                                      \
    lua_pushnumber(L, 1);                                                          \
    lua_pushnumber(L, v.x);                                                        \
    lua_settable(L, -3);                                                           \
    lua_pushnumber(L, 2);                                                          \
    lua_pushnumber(L, v.y);                                                        \
    lua_settable(L, -3);                                                           \
    return 1;                                                                      \
  }

#define ENT_LUA_VEC3_GETTER(n)                                                     \
  i32 lua_entity_get_##n(lua_State *L) {                                           \
    const Entity **ent_ptr = (const Entity **)luaL_checkudata(L, 1, "EntityMeta"); \
    const Vector3 v        = (*ent_ptr)->get_##n();                                \
    lua_createtable(L, 3, 0);                                                      \
    lua_pushnumber(L, 1);                                                          \
    lua_pushnumber(L, v.x);                                                        \
    lua_settable(L, -3);                                                           \
    lua_pushnumber(L, 2);                                                          \
    lua_pushnumber(L, v.y);                                                        \
    lua_settable(L, -3);                                                           \
    lua_pushnumber(L, 3);                                                          \
    lua_pushnumber(L, v.z);                                                        \
    lua_settable(L, -3);                                                           \
    return 1;                                                                      \
  }

#define ENT_LUA_SETTER(n, lt, rt)                                      \
  i32 lua_entity_set_##n(lua_State *L) {                               \
    Entity **ent_ptr = (Entity **)luaL_checkudata(L, 1, "EntityMeta"); \
    if (!lua_is##lt(L, 2)) {                                           \
      lua_pushnil(L);                                                  \
      return 1;                                                        \
    }                                                                  \
    rt val      = (rt)lua_to##lt(L, 2);                                \
    rt prev_val = (*ent_ptr)->get_##n();                               \
    (*ent_ptr)->set_##n(val);                                          \
    lua_push##lt(L, prev_val);                                         \
    return 1;                                                          \
  }

#define ENT_LUA_STRING_SETTER(n)                                       \
  i32 lua_entity_set_##n(lua_State *L) {                               \
    Entity **ent_ptr = (Entity **)luaL_checkudata(L, 1, "EntityMeta"); \
    if (!lua_isstring(L, 2)) {                                         \
      lua_pushnil(L);                                                  \
      return 1;                                                        \
    }                                                                  \
    std::string val      = std::string(lua_tostring(L, 2));            \
    std::string prev_val = (*ent_ptr)->get_##n();                      \
    (*ent_ptr)->set_##n(val);                                          \
    lua_pushstring(L, prev_val.c_str());                               \
    return 1;                                                          \
  }

#define ENT_LUA_VEC2_SETTER(n)                                         \
  i32 lua_entity_set_##n(lua_State *L) {                               \
    Entity **ent_ptr = (Entity **)luaL_checkudata(L, 1, "EntityMeta"); \
    const Vector2 v  = (*ent_ptr)->get_##n();                          \
    if (lua_istable(L, 2) && lua_objlen(L, 2) == 2) {                  \
      lua_rawgeti(L, 2, 1);                                            \
      const f32 x = (f32)luaL_checknumber(L, -1);                      \
      lua_rawgeti(L, 2, 2);                                            \
      const f32 y = (f32)luaL_checknumber(L, -1);                      \
      (*ent_ptr)->set_##n({ x, y });                                   \
    } else if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {             \
      const f32 x = (f32)lua_tonumber(L, 2);                           \
      const f32 y = (f32)lua_tonumber(L, 3);                           \
      (*ent_ptr)->set_##n({ x, y });                                   \
    } else {                                                           \
      lua_pushnil(L);                                                  \
      return 1;                                                        \
    }                                                                  \
    lua_createtable(L, 2, 0);                                          \
    lua_pushnumber(L, 1);                                              \
    lua_pushnumber(L, v.x);                                            \
    lua_settable(L, -3);                                               \
    lua_pushnumber(L, 2);                                              \
    lua_pushnumber(L, v.y);                                            \
    lua_settable(L, -3);                                               \
    return 1;                                                          \
  }

#define ENT_LUA_VEC3_SETTER(n)                                                   \
  i32 lua_entity_set_##n(lua_State *L) {                                         \
    Entity **ent_ptr = (Entity **)luaL_checkudata(L, 1, "EntityMeta");           \
    const Vector3 v  = (*ent_ptr)->get_##n();                                    \
    if (lua_istable(L, 2) && lua_objlen(L, 2) == 3) {                            \
      lua_rawgeti(L, 2, 1);                                                      \
      const f32 x = (f32)luaL_checknumber(L, -1);                                \
      lua_rawgeti(L, 2, 2);                                                      \
      const f32 y = (f32)luaL_checknumber(L, -1);                                \
      lua_rawgeti(L, 2, 3);                                                      \
      const f32 z = (f32)luaL_checknumber(L, -1);                                \
      (*ent_ptr)->set_##n({ x, y, z });                                          \
    } else if (lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4)) { \
      const f32 x = (f32)lua_tonumber(L, 2);                                     \
      const f32 y = (f32)lua_tonumber(L, 3);                                     \
      const f32 z = (f32)lua_tonumber(L, 4);                                     \
      (*ent_ptr)->set_##n({ x, y, z });                                          \
    } else {                                                                     \
      lua_pushnil(L);                                                            \
      return 1;                                                                  \
    }                                                                            \
    lua_createtable(L, 2, 0);                                                    \
    lua_pushnumber(L, 1);                                                        \
    lua_pushnumber(L, v.x);                                                      \
    lua_settable(L, -3);                                                         \
    lua_pushnumber(L, 2);                                                        \
    lua_pushnumber(L, v.y);                                                      \
    lua_settable(L, -3);                                                         \
    lua_pushnumber(L, 3);                                                        \
    lua_pushnumber(L, v.z);                                                      \
    lua_settable(L, -3);                                                         \
    return 1;                                                                    \
  }

/* Actual entity functions */

ENT_LUA_GETTER(active, boolean)
ENT_LUA_GETTER(should_collide, boolean)
ENT_LUA_GETTER(should_remove, boolean)
ENT_LUA_GETTER(should_sort, boolean)
ENT_LUA_GETTER(animated, boolean)
ENT_LUA_GETTER(visible, boolean)
ENT_LUA_GETTER(ready, boolean)
ENT_LUA_GETTER(updated_at, number)
ENT_LUA_GETTER(lifetime, number)
ENT_LUA_GETTER(id, number)
ENT_LUA_GETTER(ent_class, number)
ENT_LUA_GETTER(angular_velocity, number)
ENT_LUA_GETTER(current_anim, number)
ENT_LUA_GETTER(entity_index, number)
ENT_LUA_GETTER(angle, number)
ENT_LUA_STRING_GETTER(texture_path)
ENT_LUA_VEC2_GETTER(uv_offset)
ENT_LUA_VEC2_GETTER(uv_size)
ENT_LUA_VEC2_GETTER(scale)
ENT_LUA_VEC3_GETTER(aabb_max)
ENT_LUA_VEC3_GETTER(aabb_min)
ENT_LUA_VEC3_GETTER(velocity)
ENT_LUA_VEC3_GETTER(axis)
ENT_LUA_VEC3_GETTER(pos)

ENT_LUA_SETTER(lifetime, number, f64)
ENT_LUA_SETTER(current_anim, number, u32)
ENT_LUA_SETTER(ent_class, number, EntClass)
ENT_LUA_SETTER(should_collide, boolean, bool)
ENT_LUA_SETTER(should_remove, boolean, bool)
ENT_LUA_SETTER(should_sort, boolean, bool)
ENT_LUA_SETTER(animated, boolean, bool)
ENT_LUA_SETTER(visible, boolean, bool)
ENT_LUA_SETTER(active, boolean, bool)
ENT_LUA_SETTER(ready, boolean, bool)
ENT_LUA_STRING_SETTER(texture_path)
ENT_LUA_VEC2_SETTER(uv_offset)
ENT_LUA_VEC2_SETTER(uv_size)
ENT_LUA_VEC2_SETTER(scale)
ENT_LUA_VEC3_SETTER(pos)

i32 lua_entity_rotate_continuous(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  if (lua_istable(state, 2) && lua_objlen(state, 2) == 3 && lua_isnumber(state, 3)) {
    lua_rawgeti(state, 2, 1);
    const f32 x = (f32)luaL_checknumber(state, -1);
    lua_rawgeti(state, 2, 2);
    const f32 y = (f32)luaL_checknumber(state, -1);
    lua_rawgeti(state, 2, 3);
    const f32 z   = (f32)luaL_checknumber(state, -1);
    const f32 vel = (f32)luaL_checknumber(state, 3);
    (*ent_ptr)->rotate_continuous({ x, y, z }, vel);
    lua_pushboolean(state, 1);
  } else
    lua_pushboolean(state, 0);
  return 1;
}

i32 lua_entity_rotate(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  if (lua_istable(state, 2) && lua_objlen(state, 2) == 3 && lua_isnumber(state, 3)) {
    lua_rawgeti(state, 2, 1);
    const f32 x = (f32)luaL_checknumber(state, -1);
    lua_rawgeti(state, 2, 2);
    const f32 y = (f32)luaL_checknumber(state, -1);
    lua_rawgeti(state, 2, 3);
    const f32 z     = (f32)luaL_checknumber(state, -1);
    const f32 angle = (f32)luaL_checknumber(state, 3);
    (*ent_ptr)->rotate({ x, y, z }, angle);
    lua_pushboolean(state, 1);
  } else
    lua_pushboolean(state, 0);
  return 1;
}

i32 lua_entity_collides_with(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  if (lua_isuserdata(state, 2)) {
    Entity **ent2_ptr = (Entity **)luaL_checkudata(state, 2, "EntityMeta");
    lua_pushboolean(state, (*ent_ptr)->collides_with(*ent2_ptr));
  } else
    lua_pushboolean(state, 0);
  return 1;
}

i32 lua_entity_set_velocity(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  if (lua_istable(state, 2) && lua_objlen(state, 2) == 3) {
    lua_rawgeti(state, 2, 1);
    const f32 x = (f32)luaL_checknumber(state, -1);
    lua_rawgeti(state, 2, 2);
    const f32 y = (f32)luaL_checknumber(state, -1);
    lua_rawgeti(state, 2, 3);
    const f32 z = (f32)luaL_checknumber(state, -1);
    (*ent_ptr)->set_velocity({ x, y, z });
    lua_pushboolean(state, 1);
  } else
    lua_pushboolean(state, 0);
  return 1;
}

i32 lua_entity_set_velocity_x(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  if (lua_isnumber(state, 2)) {
    const f32 vel = (f32)luaL_checknumber(state, 2);
    (*ent_ptr)->set_velocity_x(vel);
    lua_pushboolean(state, 1);
  } else
    lua_pushboolean(state, 0);
  return 1;
}

i32 lua_entity_set_velocity_y(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  if (lua_isnumber(state, 2)) {
    const f32 vel = (f32)luaL_checknumber(state, 2);
    (*ent_ptr)->set_velocity_y(vel);
    lua_pushboolean(state, 1);
  } else
    lua_pushboolean(state, 0);
  return 1;
}

i32 lua_entity_set_parent(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  if (lua_isuserdata(state, 2)) {
    Entity **ent2_ptr = (Entity **)luaL_checkudata(state, 2, "EntityMeta");
    (*ent_ptr)->set_parent(*ent2_ptr);
  }

  return 0;
}

i32 lua_entity_should_be_removed(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  lua_pushboolean(state, (*ent_ptr)->should_be_removed());

  return 1;
}

i32 lua_entity_is_valid(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  lua_pushboolean(state, (*ent_ptr)->is_valid());

  return 1;
}

i32 lua_entity_update(lua_State *state) {
  Entity **ent_ptr = (Entity **)luaL_checkudata(state, 1, "EntityMeta");

  (*ent_ptr)->update(platform_time_f64());

  return 0;
}

/* Reg stuff */

static const luaL_Reg lua_ent_functions[] = {
  { "get_active", lua_entity_get_active },
  { "get_should_collide", lua_entity_get_should_collide },
  { "get_should_remove", lua_entity_get_should_remove },
  { "get_should_sort", lua_entity_get_should_sort },
  { "get_animated", lua_entity_get_animated },
  { "get_visible", lua_entity_get_visible },
  { "get_ready", lua_entity_get_ready },
  { "get_updated_at", lua_entity_get_updated_at },
  { "get_lifetime", lua_entity_get_lifetime },
  { "get_id", lua_entity_get_id },
  { "get_ent_class", lua_entity_get_ent_class },
  { "get_angular_velocity", lua_entity_get_angular_velocity },
  { "get_current_anim", lua_entity_get_current_anim },
  { "get_entity_index", lua_entity_get_entity_index },
  { "get_angle", lua_entity_get_angle },
  { "get_texture_path", lua_entity_get_texture_path },
  { "get_uv_offset", lua_entity_get_uv_offset },
  { "get_uv_size", lua_entity_get_uv_size },
  { "get_scale", lua_entity_get_scale },
  { "get_aabb_max", lua_entity_get_aabb_max },
  { "get_aabb_min", lua_entity_get_aabb_min },
  { "get_velocity", lua_entity_get_velocity },
  { "get_axis", lua_entity_get_axis },
  { "get_pos", lua_entity_get_pos },
  { "set_lifetime", lua_entity_set_lifetime },
  { "set_current_anim", lua_entity_set_current_anim },
  { "set_ent_class", lua_entity_set_ent_class },
  { "set_should_collide", lua_entity_set_should_collide },
  { "set_should_remove", lua_entity_set_should_remove },
  { "set_should_sort", lua_entity_set_should_sort },
  { "set_animated", lua_entity_set_animated },
  { "set_visible", lua_entity_set_visible },
  { "set_active", lua_entity_set_active },
  { "set_ready", lua_entity_set_ready },
  { "set_uv_offset", lua_entity_set_uv_offset },
  { "set_uv_size", lua_entity_set_uv_size },
  { "set_scale", lua_entity_set_scale },
  { "set_pos", lua_entity_set_pos },
  { "rotate_continuous", lua_entity_rotate_continuous },
  { "collides_with", lua_entity_collides_with },
  { "rotate", lua_entity_rotate },
  { "set_parent", lua_entity_set_parent },
  { "set_velocity", lua_entity_set_velocity },
  { "set_velocity_x", lua_entity_set_velocity_x },
  { "set_velocity_y", lua_entity_set_velocity_y },
  { "set_texture_path", lua_entity_set_texture_path },
  { "update", lua_entity_update },
  { "should_be_removed", lua_entity_should_be_removed },
  { "is_valid", lua_entity_is_valid },
  { NULL, NULL } /* sentinel */
};

void lua_push_entity(lua_State *state, Entity *ent) {
  Entity **ent_ptr = (Entity **)lua_newuserdata(state, sizeof(Entity *));
  *ent_ptr         = ent;

  if (luaL_newmetatable(state, "EntityMeta")) {
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    luaL_register(state, 0, lua_ent_functions);
  }

  lua_setmetatable(state, -2);
}
