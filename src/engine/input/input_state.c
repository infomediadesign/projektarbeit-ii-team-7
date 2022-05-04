#include "input_state.h"

#include <string.h>

InputState *input_state_default() {
  InputState *state = (InputState *)calloc(1, sizeof(state));

  state->top_binding = 0U;

  return state;
}

void input_state_free(InputState *state) { free(state); }

void input_bind(InputState *state, i32 key, u32 cmd) { state->bindings[state->top_binding++] = (KeyBind) { key, cmd }; }

void input_flush(InputState *state) {
  memset(state->commands, 0, sizeof(u32) * 256);

  state->command_count    = 0U;
  state->mouse_last       = state->mouse;
  state->left_stick_last  = state->left_stick;
  state->right_stick_last = state->right_stick;
  state->mouse            = (Axis2) { 0.0, 0.0 };
  state->left_stick       = (Axis2) { 0.0, 0.0 };
  state->right_stick      = (Axis2) { 0.0, 0.0 };
}
