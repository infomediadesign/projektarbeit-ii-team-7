#include "argparse.h"

inline u8 arg_is_valid(const i32 i, const i32 argc, const char *argv[]) {
  return i + 1 < argc && argv[i + 1][0] != '-';
}

void input_parse_args(GameState *const state, const i32 argc,
                      const char *argv[]) {
  for (i32 i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-tickrate") == 0 && arg_is_valid(i, argc, argv)) {
      const u32 tickrate = (u32)strtol(argv[i + 1], NULL, 10);

      if (tickrate > 0) {
        state->tickrate = tickrate;
        i++;
      }
    } else if (strcmp(argv[i], "-fps") == 0 && arg_is_valid(i, argc, argv)) {
      state->fps_max = (u32)strtol(argv[i + 1], NULL, 10);
      i++;
    } else if (strcmp(argv[i], "-width") == 0 && arg_is_valid(i, argc, argv)) {
      state->width = (u32)strtol(argv[i + 1], NULL, 10);
      i++;
    } else if (strcmp(argv[i], "-height") == 0 && arg_is_valid(i, argc, argv)) {
      state->height = (u32)strtol(argv[i + 1], NULL, 10);
      i++;
    } else if (strcmp(argv[i], "-debug") == 0) {
      game_add_flag(state, GS_DEBUG);
    } else if (strcmp(argv[i], "-platform") == 0 &&
               arg_is_valid(i, argc, argv)) {
#ifndef _WIN32
      state->preferred_platform = argv[i + 1];
#endif
      i++;
    } else {
      printf(
          "[Miniflow] Unrecognized command-line parameter '%s', ignoring...\n",
          argv[i]);
    }
  }
}
