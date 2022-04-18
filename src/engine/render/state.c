#include "state.h"

RenderState *render_state_init() {
  RenderState *state = (RenderState *)malloc(sizeof(RenderState));

  state->current_frame = 0;
  state->current_swapchain_image = 0;

  return state;
}

void render_state_destroy(RenderState *state) {
  glfwDestroyWindow(state->window);
  free(state->instance);
  free(state);
}

void render_state_create_window(RenderState *state) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  state->window = glfwCreateWindow(640, 480, "Miniflow", NULL, NULL);
}

u8 render_state_should_close(RenderState *state) {
  return glfwWindowShouldClose(state->window);
}
