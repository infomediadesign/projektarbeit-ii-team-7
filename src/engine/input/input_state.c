#include "input_state.h"

#include <string.h>

InputState *input_state_default() {
  InputState *state = (InputState *)calloc(1, sizeof(InputState));

  state->bindings    = (KeyBind *)calloc(1024, sizeof(KeyBind));
  state->commands    = (i32 *)calloc(1024, sizeof(i32));
  state->raw         = (u32 *)calloc(1024, sizeof(u32));
  state->top_binding = 0U;

  return state;
}

void input_state_free(InputState *state) {
  free(state->bindings);
  free(state->commands);
  free(state->raw);
  free(state);
}

void input_bind(InputState *state, const i32 key, const i32 cmd) {
  state->bindings[state->top_binding++] = (KeyBind) { key, cmd };
}

void input_flush(InputState *state) {
  memcpy(state->gamepad_last, state->gamepad, sizeof(u8) * (GLFW_GAMEPAD_BUTTON_LAST + 1));
  memset(state->commands, 0, sizeof(i32) * 1024);
  memset(state->raw, 0, sizeof(u32) * 1024);
  memset(state->gamepad, 0, sizeof(u8) * (GLFW_GAMEPAD_BUTTON_LAST + 1));

  state->command_count      = 0U;
  state->raw_count          = 0U;
  state->mouse_last         = state->mouse;
  state->left_stick_last    = state->left_stick;
  state->right_stick_last   = state->right_stick;
  state->left_trigger_last  = state->left_trigger;
  state->right_trigger_last = state->right_trigger;
  state->left_trigger       = 0.0f;
  state->right_trigger      = 0.0f;
}

void input_push(InputState *state, const i32 cmd) { state->commands[state->command_count++] = cmd; }

void input_push_raw(InputState *state, const u32 raw) { state->raw[state->raw_count++] = raw; }

KeyBind input_find_binding(const InputState *state, const u32 key) {
  for (u32 i = 0; i < state->top_binding; i++)
    if (state->bindings[i].key == key)
      return state->bindings[i];

  return (KeyBind) { 0, 0 };
}
