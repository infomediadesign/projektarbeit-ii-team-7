#ifndef __GAME_INTERFACE_H
#define __GAME_INTERFACE_H

/**
 * This file is an interface between the Engine (C)
 * and the Game (C++). The engine will call these
 * functions as events occur in the engine. The game
 * is expected to react to these events and perform logic.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>

void game_initialize(GameState *state);
void game_tick(GameState *state, mutex_t *lock);
void game_lazy_tick(GameState *state, mutex_t *lock);
void game_paused_tick(GameState *state, mutex_t *lock);
void game_adjust_renderables(GameState *state, mutex_t *lock, Renderable *renderables, const u32 renderables_count);

#ifdef __cplusplus
}
#endif

#endif
