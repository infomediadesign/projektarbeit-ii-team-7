#include "argparse.h"

void input_parse_args(GameState *const state, const int argc,
                      const char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-tickrate") == 0 && i + 1 < argc) {
      const uint32_t tickrate = (uint32_t)atoi(argv[i + 1]);

      if (tickrate > 0) {
        state->tickrate = tickrate;
        i++;
      }
    } else if (strcmp(argv[i], "-fps") == 0 && i + 1 < argc) {
      state->fps_max = (uint32_t)atoi(argv[i + 1]);
      i++;
    } else if (strcmp(argv[i], "-debug") == 0) {
      game_add_flag(state, GS_DEBUG);
    }
  }
}
