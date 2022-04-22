#ifndef __GAME_GAME_H
#define __GAME_GAME_H

#include <engine/platform.h>
#include <engine/state/state.h>
#include <engine/types.h>

/**
 * This file is an interface between the Engine (C)
 * and the Game (C++). The engine will call these
 * functions as events occur in the engine. The game
 * is expected to read these events and perform its logic.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

void game_initialize(GameState *state);
void game_tick(GameState *state, mutex_t *lock);
void game_lazy_tick(GameState *state, mutex_t *lock);

#ifdef __cplusplus
}
#endif

#endif
