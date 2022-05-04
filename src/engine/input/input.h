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

int input_perform(void *args);

#ifdef __cplusplus
}
#endif

#endif
