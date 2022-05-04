#include "input.h"

#include <GLFW/glfw3.h>

void render_state_refresh_callback(GLFWwindow *window) {}

void render_state_resize_callback(GLFWwindow *window, i32 width, i32 height) {}

void render_state_key_callback(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods) {}

int input_perform(void *args) {
  ThreadData *const td     = (ThreadData *)args;
  GameState *const state   = (GameState *)td->state;
  GLFWwindow *const window = (GLFWwindow *)state->window;

  /* Avoid the race condition between render thread and this thread */
  while (window == NULL)
    platform_sleep(1);

  glfwSetWindowRefreshCallback(window, render_state_refresh_callback);
  glfwSetFramebufferSizeCallback(window, render_state_resize_callback);
  glfwSetKeyCallback(window, render_state_key_callback);

  while (!game_should_exit(state)) {
    glfwPollEvents();

    platform_sleep(5);
  }

  return 0;
}
