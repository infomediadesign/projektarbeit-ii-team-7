#ifndef __GAME_LUA_ENTITY_H
#define __GAME_LUA_ENTITY_H

#include <game/entities/entity.h>
#include <lua.hpp>

void lua_push_entity(lua_State *state, Entity *ent);

#endif
