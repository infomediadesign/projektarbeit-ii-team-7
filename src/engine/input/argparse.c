#include "argparse.h"

void input_parse_args(GameState *const state, const int argc,
                      const char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-tickrate") == 0 && i + 1 < argc) {
      const u32 tickrate = (u32)strtol(argv[i + 1], NULL, 10);

      if (tickrate > 0) {
        state->tickrate = tickrate;
        i++;
      }
    } else if (strcmp(argv[i], "-fps") == 0 && i + 1 < argc) {
      state->fps_max = (u32)strtol(argv[i + 1], NULL, 10);
      i++;
    } else if (strcmp(argv[i], "-debug") == 0) {
      game_add_flag(state, GS_DEBUG);
    } else if (strcmp(argv[i], "-platform") == 0 && i + 1 < argc) {
#ifndef _WIN32
      state->preferred_platform = argv[i + 1];
#endif
    }
  }
}
