#include "argparse.h"

void input_parse_args(GameState *const state, const int argc,
                      const char *argv[]) {
  int skip = 0;

  for (int i = 1; i < argc; i++) {
    if (skip > 0) {
      skip--;
      continue;
    }

    if (strcmp(argv[i], "-tickrate") == 0 && i + 1 < argc) {
      uint32_t tickrate = (uint32_t)atoi(argv[i + 1]);

      if (tickrate > 0) {
        state->tickrate = tickrate;
        skip = 1;
      }
    } else if (strcmp(argv[i], "-fps") == 0 && i + 1 < argc) {
      state->fps_max = (uint32_t)atoi(argv[i + 1]);
      skip = 1;
    } else if (strcmp(argv[i], "-debug") == 0) {
      game_add_flag(state, GS_DEBUG);
    }
  }
}
