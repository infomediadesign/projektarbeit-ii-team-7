#include "render_state.h"

RenderState *render_state_init() {
  RenderState *state = (RenderState *)malloc(sizeof(RenderState));

  state->current_frame = 0;
  state->current_swapchain_image = 0;
  state->window_width = 768;
  state->window_height = 432;
  state->debug = 0;

  return state;
}

void render_state_destroy(RenderState *state) {
  glfwDestroyWindow(state->window);
  free(state->swapchain_images);
  free(state->instance);
  free(state);
}

void render_state_refresh_callback(GLFWwindow *window) {}

void render_state_resize_callback(GLFWwindow *window, i32 width, i32 height) {}

void render_state_key_callback(GLFWwindow *window, i32 key, i32 scancode,
                               i32 action, i32 mods) {}

void render_state_create_window(RenderState *state) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  state->window = glfwCreateWindow(state->window_width, state->window_height,
                                   "Miniflow", NULL, NULL);

  glfwSetWindowUserPointer(state->window, state);
  glfwSetWindowRefreshCallback(state->window, render_state_refresh_callback);
  glfwSetFramebufferSizeCallback(state->window, render_state_resize_callback);
  glfwSetKeyCallback(state->window, render_state_key_callback);
}

u8 render_state_should_close(RenderState *state) {
  return glfwWindowShouldClose(state->window);
}
