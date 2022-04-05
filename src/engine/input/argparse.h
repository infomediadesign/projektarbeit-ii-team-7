#ifndef __ENGINE_INPUT_ARGPARSE_H
#define __ENGINE_INPUT_ARGPARSE_H

#include "../state/state.h"
#include <string.h>

void input_parse_args(GameState *const state, const int argc,
                      const char *argv[]);

#endif
