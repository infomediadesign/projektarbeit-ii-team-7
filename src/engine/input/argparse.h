#ifndef __ENGINE_INPUT_ARGPARSE_H
#define __ENGINE_INPUT_ARGPARSE_H

#include "../state/state.h"
#include "../types/numeric.h"
#include <string.h>

void input_parse_args(GameState *const state, const i32 argc,
                      const char *argv[]);

#endif
