#include "render.h"
#include <raylib.h>

int render_perform(void *args) {
  ThreadData *const td = (ThreadData *)args;
  GameState *const state = (GameState *)td->state;

  const Color black = { 0, 0, 0, 255 };

  InitWindow(state->width, state->height, "Miniflow");
  SetTargetFPS(state->fps_max);

  while (!game_should_exit(state)) {
    BeginDrawing();
      ClearBackground(black);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
