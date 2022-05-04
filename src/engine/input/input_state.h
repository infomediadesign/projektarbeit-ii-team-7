#ifndef __ENGINE_INPUT_INPUT_STATE_H
#define __ENGINE_INPUT_INPUT_STATE_H

#include <engine/types/numeric.h>

typedef struct KeyBind {
  u32 key;
  u32 cmd;
} KeyBind;

typedef struct Axis2 {
  double x;
  double y;
} Axis2;

typedef struct InputState {
  KeyBind bindings[1024];
  u32 top_binding;
  u32 commands[256]; /* 256 commands at a time should be enough */
  u32 command_count;
  Axis2 mouse;
  Axis2 mouse_last;
  Axis2 left_stick;
  Axis2 left_stick_last;
  Axis2 right_stick;
  Axis2 right_stick_last;
} InputState;

InputState *input_state_default();
void input_state_free(InputState *state);
void input_bind(InputState *state, i32 key, u32 cmd);
void input_flush(InputState *state);

#endif
