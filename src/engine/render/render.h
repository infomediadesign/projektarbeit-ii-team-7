#ifndef __ENGINE_RENDER_RENDER_H
#define __ENGINE_RENDER_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../platform.h"
#include "../state/state.h"
#include "../util.h"
#include "geyser.h"
#include "memory.h"
#include "render_state.h"
#include "renderable.h"

#include <string.h>

/**
 * @brief Manages the game window and performs rendering.
 *
 * This is the main rendering loop. It is responsible for managing
 * the window, rendering contexts, interpolation, and so on.
 *
 * @param args ThreadData structure
 * @return int Always returns 0
 */
int render_perform(void *args);

#ifdef __cplusplus
}
#endif

#endif
