#include "text.h"

#include "helpers.h"

#define TEXT_LUA_STRING_GETTER(n)                                             \
  i32 lua_text_get_##n(lua_State *L) {                                        \
    const Text **text_ptr = (const Text **)luaL_checkudata(L, 1, "TextMeta"); \
    lua_pushstring(L, (*text_ptr)->n.c_str());                                \
    return 1;                                                                 \
  }

#define TEXT_LUA_VEC2_GETTER(n)                                               \
  i32 lua_text_get_##n(lua_State *L) {                                        \
    const Text **text_ptr = (const Text **)luaL_checkudata(L, 1, "TextMeta"); \
    const Vector2 v       = (*text_ptr)->n;                                   \
    lua_createtable(L, 0, 2);                                                 \
    LUA_TABLE_SET_NAMED(L, number, "x", v.x);                                 \
    LUA_TABLE_SET_NAMED(L, number, "y", v.y);                                 \
    return 1;                                                                 \
  }

#define TEXT_LUA_VEC4_GETTER(n)                                               \
  i32 lua_text_get_##n(lua_State *L) {                                        \
    const Text **text_ptr = (const Text **)luaL_checkudata(L, 1, "TextMeta"); \
    const Vector4 v       = (*text_ptr)->n;                                   \
    lua_createtable(L, 0, 4);                                                 \
    LUA_TABLE_SET_NAMED(L, number, "x", v.x);                                 \
    LUA_TABLE_SET_NAMED(L, number, "y", v.y);                                 \
    LUA_TABLE_SET_NAMED(L, number, "z", v.z);                                 \
    LUA_TABLE_SET_NAMED(L, number, "w", v.w);                                 \
    return 1;                                                                 \
  }

#define TEXT_LUA_STRING_SETTER(n)                                 \
  i32 lua_text_set_##n(lua_State *L) {                            \
    Text **text_ptr = (Text **)luaL_checkudata(L, 1, "TextMeta"); \
    if (!lua_isstring(L, 2)) {                                    \
      lua_pushnil(L);                                             \
      return 1;                                                   \
    }                                                             \
    std::string val      = std::string(lua_tostring(L, 2));       \
    std::string prev_val = (*text_ptr)->n;                        \
    (*text_ptr)->n       = val;                                   \
    lua_pushstring(L, prev_val.c_str());                          \
    return 1;                                                     \
  }

#define TEXT_LUA_VEC2_SETTER(n)                                   \
  i32 lua_text_set_##n(lua_State *L) {                            \
    Text **text_ptr = (Text **)luaL_checkudata(L, 1, "TextMeta"); \
    const Vector2 v = (*text_ptr)->n;                             \
    if (lua_istable(L, 2)) {                                      \
      lua_getfield(L, 2, "x");                                    \
      const f32 x = (f32)luaL_checknumber(L, -1);                 \
      lua_getfield(L, 2, "y");                                    \
      const f32 y    = (f32)luaL_checknumber(L, -1);              \
      (*text_ptr)->n = { x, y };                                  \
    } else if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {        \
      const f32 x    = (f32)lua_tonumber(L, 2);                   \
      const f32 y    = (f32)lua_tonumber(L, 3);                   \
      (*text_ptr)->n = { x, y };                                  \
    } else {                                                      \
      lua_pushnil(L);                                             \
      return 1;                                                   \
    }                                                             \
    lua_createtable(L, 0, 2);                                     \
    LUA_TABLE_SET_NAMED(L, number, "x", v.x);                     \
    LUA_TABLE_SET_NAMED(L, number, "y", v.y);                     \
    return 1;                                                     \
  }

#define TEXT_LUA_VEC4_SETTER(n)                                                                        \
  i32 lua_text_set_##n(lua_State *L) {                                                                 \
    Text **text_ptr = (Text **)luaL_checkudata(L, 1, "TextMeta");                                      \
    const Vector4 v = (*text_ptr)->n;                                                                  \
    if (lua_istable(L, 2)) {                                                                           \
      lua_getfield(L, 2, "x");                                                                         \
      const f32 x = (f32)luaL_checknumber(L, -1);                                                      \
      lua_getfield(L, 2, "y");                                                                         \
      const f32 y = (f32)luaL_checknumber(L, -1);                                                      \
      lua_getfield(L, 2, "z");                                                                         \
      const f32 z = (f32)luaL_checknumber(L, -1);                                                      \
      lua_getfield(L, 2, "w");                                                                         \
      const f32 w    = (f32)luaL_checknumber(L, -1);                                                   \
      (*text_ptr)->n = { x, y, z, w };                                                                 \
    } else if (lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) { \
      const f32 x    = (f32)lua_tonumber(L, 2);                                                        \
      const f32 y    = (f32)lua_tonumber(L, 3);                                                        \
      const f32 z    = (f32)lua_tonumber(L, 4);                                                        \
      const f32 w    = (f32)lua_tonumber(L, 5);                                                        \
      (*text_ptr)->n = { x, y, z, w };                                                                 \
    } else {                                                                                           \
      lua_pushnil(L);                                                                                  \
      return 1;                                                                                        \
    }                                                                                                  \
    lua_createtable(L, 0, 4);                                                                          \
    LUA_TABLE_SET_NAMED(L, number, "x", v.x);                                                          \
    LUA_TABLE_SET_NAMED(L, number, "y", v.y);                                                          \
    LUA_TABLE_SET_NAMED(L, number, "z", v.z);                                                          \
    LUA_TABLE_SET_NAMED(L, number, "w", v.w);                                                          \
    return 1;                                                                                          \
  }

TEXT_LUA_STRING_GETTER(text)
TEXT_LUA_VEC2_GETTER(scale)
TEXT_LUA_VEC4_GETTER(pos)
TEXT_LUA_VEC4_GETTER(color)
TEXT_LUA_STRING_SETTER(text)
TEXT_LUA_VEC2_SETTER(scale)
TEXT_LUA_VEC4_SETTER(pos)
TEXT_LUA_VEC4_SETTER(color)

i32 lua_text_active(lua_State *state) {
  Text **text_ptr = (Text **)luaL_checkudata(state, 1, "TextMeta");

  lua_pushboolean(state, (*text_ptr)->active);

  return 1;
}

static const luaL_Reg lua_text_functions[] = {
  { "get_active", lua_text_active },
  { "active", lua_text_active },
  { "get_text", lua_text_get_text },
  { "get_scale", lua_text_get_scale },
  { "get_pos", lua_text_get_pos },
  { "get_color", lua_text_get_color },
  { "set_text", lua_text_set_text },
  { "set_scale", lua_text_set_scale },
  { "set_pos", lua_text_set_pos },
  { "set_color", lua_text_set_color },
  { NULL, NULL } /* sentinel */
};

void lua_push_text(lua_State *state, Text *txt) {
  Text **txt_ptr = (Text **)lua_newuserdata(state, sizeof(Text *));
  *txt_ptr       = txt;

  if (luaL_newmetatable(state, "TextMeta")) {
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    luaL_register(state, 0, lua_text_functions);
  }

  lua_setmetatable(state, -2);
}
