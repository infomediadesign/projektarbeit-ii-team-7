#ifndef __GAME_LUA_HELPERS_H
#define __GAME_LUA_HELPERS_H

#define LUA_EVENT_RUN(L, id)  \
  lua_getglobal(L, "event");  \
  lua_getfield(L, -1, "run"); \
  lua_remove(L, -2);          \
  lua_pushstring(L, id);
#define LUA_EVENT_CALL(L, args, res) lua_call(L, args + 1, res);
#define LUA_TABLE_INSERT(L, t, idx, val) \
  lua_pushnumber(L, idx);                \
  lua_push##t(L, val);                   \
  lua_settable(L, -3);
#define LUA_TABLE_SET_NAMED(L, t, n, val) \
  lua_pushstring(L, n);                   \
  lua_push##t(L, val);                    \
  lua_settable(L, -3);

#endif
