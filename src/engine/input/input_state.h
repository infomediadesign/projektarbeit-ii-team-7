#ifndef __ENGINE_INPUT_INPUT_STATE_H
#define __ENGINE_INPUT_INPUT_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <engine/types/numeric.h>

typedef struct KeyBind {
  i32 key;
  i32 cmd;
} KeyBind;

typedef struct Axis2 {
  double x;
  double y;
} Axis2;

typedef struct InputState {
  KeyBind *bindings;
  u32 top_binding;
  i32 *commands;
  u32 command_count;
  u32 *raw;
  u32 raw_count;
  Axis2 mouse;
  Axis2 mouse_last;
  Axis2 left_stick;
  Axis2 left_stick_last;
  Axis2 right_stick;
  Axis2 right_stick_last;
} InputState;

InputState *input_state_default();
void input_state_free(InputState *state);
void input_bind(InputState *state, const i32 key, const i32 cmd);
void input_flush(InputState *state);
void input_push(InputState *state, const i32 cmd);
void input_push_raw(InputState *state, const u32 raw);
KeyBind input_find_binding(const InputState *state, const u32 key);

#ifdef __cplusplus
}
#endif

#endif
