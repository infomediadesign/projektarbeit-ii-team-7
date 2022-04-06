#ifndef __ENGINE_LOGIC_LOGIC_H
#define __ENGINE_LOGIC_LOGIC_H

#include "../platform.h"
#include "../state/state.h"
#include "../util.h"

/**
 * @brief Main function for game logic.
 *
 * This function runs in its own thread `tickrate` times
 * every second. It handles things like logic, position adjustments,
 * and manages entities.
 *
 * @param args Pointer to ThreadData structure passed to the thread
 * @return int Always returns 0
 */
int logic_perform(void *args);

#endif
