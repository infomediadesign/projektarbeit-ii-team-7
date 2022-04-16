#ifndef __ENGINE_RENDER_RENDER_H
#define __ENGINE_RENDER_RENDER_H

#include "../platform.h"
#include "../state/state.h"
#include "../util.h"
#include "geyser.h"
#include "state.h"

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

#endif
