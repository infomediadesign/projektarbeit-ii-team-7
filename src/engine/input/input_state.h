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
  f64 x;
  f64 y;
} Axis2;

typedef struct InputState {
  KeyBind *bindings;
  i32 *commands;
  u32 *raw;
  Axis2 mouse;
  Axis2 mouse_last;
  Axis2 left_stick;
  Axis2 left_stick_last;
  Axis2 right_stick;
  Axis2 right_stick_last;
  u32 top_binding;
  u32 command_count;
  u32 raw_count;
} InputState;

/**
 * @brief Allocates and sets a new input state structure to default values.
 *
 * @warning You must free the memory yourself.
 *
 * @return InputState* The newly created and allocated input state.
 */
InputState *input_state_default();

/**
 * @brief Frees the memory belonging to the input state.
 *
 * @param state The state to destroy.
 */
void input_state_free(InputState *state);

/**
 * @brief Creates a key binding.
 *
 * You may combine keycodes with bitmask values, such as:
 *
 * ```
 * input_bind(state, MF_KEY_UP | MF_KEY_PRESSED, Cmd::Forward);
 * ```
 *
 * @param state The input state.
 * @param key The key to react to.
 * @param cmd The command to write when the key is registered.
 */
void input_bind(InputState *state, const i32 key, const i32 cmd);

/**
 * @brief Flushes the input buffer.
 *
 * This simply purges everything from the buffer. Call this function
 * once you are done reading the input.
 *
 * @param state The input state.
 */
void input_flush(InputState *state);

/**
 * @brief Push a command onto the input buffer.
 *
 * @param state The input state.
 * @param cmd The command to push.
 */
void input_push(InputState *state, const i32 cmd);

/**
 * @brief Push a raw keycode onto the input buffer.
 *
 * @param state The input state.
 * @param raw The unicode key code.
 */
void input_push_raw(InputState *state, const u32 raw);

/**
 * @brief Finds command for the key.
 *
 * @param state The input state.
 * @param key The pressed key.
 * @return KeyBind The binding associated with that key.
 */
KeyBind input_find_binding(const InputState *state, const u32 key);

#ifdef __cplusplus
}
#endif

#endif
