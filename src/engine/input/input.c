#include "input.h"

#include <GLFW/glfw3.h>
#include <game/interface.h>

static InputState *IS;

void input_refresh_callback(GLFWwindow *window) {}

void input_resize_callback(GLFWwindow *window, i32 width, i32 height) {}

void input_key_callback(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods) {
  if (key > -1) {
    i32 real_key = (action << 30) + (mods << 24) + key;

    KeyBind bind = input_find_binding(IS, real_key);

    if (bind.cmd != 0)
      input_push(IS, bind.cmd);
  }
}

void input_char_callback(GLFWwindow *window, u32 codepoint) { input_push_raw(IS, codepoint); }

int input_perform(void *args) {
  ThreadData *const td   = (ThreadData *)args;
  GameState *const state = (GameState *)td->state;
  mutex_t *lock          = (mutex_t *)td->lock;
  GLFWwindow *window     = (GLFWwindow *)state->window;
  IS                     = input_state_default();

  /* Avoid the race condition between render thread and this thread */
  while (window == NULL) {
    window = (GLFWwindow *)state->window;
    platform_sleep(1);
  }

  glfwSetWindowRefreshCallback(window, input_refresh_callback);
  glfwSetFramebufferSizeCallback(window, input_resize_callback);
  glfwSetKeyCallback(window, input_key_callback);

  game_create_bindings(state, lock, IS);

  while (!game_should_exit(state)) {
#ifndef _WIN32
    glfwPollEvents();
#endif

    platform_sleep(2);
  }

  input_state_free(IS);

  return 0;
}
