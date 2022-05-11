#ifndef __ENGINE_INPUT_ARGPARSE_H
#define __ENGINE_INPUT_ARGPARSE_H

#include "../state/state.h"
#include "../types/numeric.h"

#include <string.h>

/**
 * @brief Parses commandline arguments and writes relevant data to the game state.
 * 
 * @param state The game state.
 * @param argc The amount of arguments in the array.
 * @param argv The array of argument strings.
 */
void input_parse_args(GameState *const state, const i32 argc, const char *argv[]);

#endif
