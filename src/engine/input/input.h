#ifndef __ENGINE_INPUT_INPUT_H
#define __ENGINE_INPUT_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../platform.h"
#include "../state/state.h"
#include "../util.h"
#include "input_state.h"
#include "keys.h"

/**
 * @brief Performs the input polling and registers window callbacks.
 *
 * @warning On Windows, the input polling is performed in the render thread!
 *
 * @param args Thread arguments.
 * @return int Always returns 0.
 */
int input_perform(void *args);

#ifdef __cplusplus
}
#endif

#endif
