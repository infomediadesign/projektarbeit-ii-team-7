#ifndef __GAME_LUA_TEXT_H
#define __GAME_LUA_TEXT_H

#include <engine/platform.h>
#include <game/text.h>
#include <lua.hpp>

void lua_push_text(lua_State *state, Text *txt);

#endif
