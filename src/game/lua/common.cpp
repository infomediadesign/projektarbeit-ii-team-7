#include "common.h"

i32 lua_platform_time(lua_State *state) {
  lua_pushnumber(state, platform_time_f64());
  return 1;
}

i32 lua_platform_time_sec(lua_State *state) {
  lua_pushnumber(state, platform_time_sec());
  return 1;
}

i32 lua_platform_time_usec(lua_State *state) {
  lua_pushnumber(state, platform_time_usec());
  return 1;
}

i32 lua_platform_sleep(lua_State *state) {
  u64 t = (u64)luaL_checknumber(state, 1);
  platform_sleep(t);
  return 0;
}

i32 lua_platform_usleep(lua_State *state) {
  u64 t = (u64)luaL_checknumber(state, 1);
  platform_usleep(t);
  return 0;
}

i32 lua_platform_nsleep(lua_State *state) {
  u64 t = (u64)luaL_checknumber(state, 1);
  platform_nsleep(t);
  return 0;
}

static const luaL_Reg lua_platform_lib[] = {
  { "time", lua_platform_time },
  { "timesec", lua_platform_time_sec },
  { "timeusec", lua_platform_time_usec },
  { "sleep", lua_platform_sleep },
  { "usleep", lua_platform_usleep },
  { "nsleep", lua_platform_nsleep },
  { NULL, NULL } /* sentinel */
};

i32 lua_util_lerp(lua_State *state) {
  f64 d = luaL_checknumber(state, 1);
  f64 f = luaL_checknumber(state, 2);
  f64 t = luaL_checknumber(state, 3);
  lua_pushnumber(state, util_lerp_f64(d, f, t));
  return 1;
}

i32 lua_util_kibibytes(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_kibibytes(v));
  return 1;
}

i32 lua_util_mebibytes(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_mebibytes(v));
  return 1;
}

i32 lua_util_gibibytes(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_gibibytes(v));
  return 1;
}

i32 lua_util_kbytes(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_kbytes(v));
  return 1;
}

i32 lua_util_mbytes(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_mbytes(v));
  return 1;
}

i32 lua_util_gbytes(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_gbytes(v));
  return 1;
}

i32 lua_util_radians(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_radians(v));
  return 1;
}

i32 lua_util_degrees(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_degrees(v));
  return 1;
}

i32 lua_util_meters(lua_State *state) {
  f64 v = luaL_checknumber(state, 1);
  lua_pushnumber(state, util_meters(v));
  return 1;
}

static const luaL_Reg lua_util_lib[] = {
  { "lerp", lua_util_lerp },
  { "kibibytes", lua_util_kibibytes },
  { "mebibytes", lua_util_mebibytes },
  { "gibibytes", lua_util_gibibytes },
  { "kbytes", lua_util_kbytes },
  { "mbytes", lua_util_mbytes },
  { "gbytes", lua_util_gbytes },
  { "radians", lua_util_radians },
  { "degrees", lua_util_degrees },
  { "meters", lua_util_meters },
  { NULL, NULL } /* sentinel */
};

i32 lua_dostring(lua_State *state) { return 0; }

void lua_register_common(lua_State *state) {
  luaL_register(state, "platform", lua_platform_lib);
  luaL_register(state, "util", lua_util_lib);

  lua_pushcfunction(state, lua_dostring);
  lua_setglobal(state, "dostring");
}
