#include "render_state.h"

RenderState *render_state_init() {
  RenderState *state = (RenderState *)malloc(sizeof(RenderState));

  const Matrix4 null_mat4 = {
    { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }
  };

  state->current_frame           = 0;
  state->current_swapchain_image = 0;
  state->window_width            = 768.0f;
  state->window_height           = 432.0f;
  state->debug                   = 0;
  state->rendering               = 0;
  state->camera_transform        = null_mat4;
  state->render_scale            = RENDER_SCALE;

  state->camera_transform.x[0] = state->render_scale;
  state->camera_transform.y[1] = 768.0f / 432.0f * state->render_scale;
  state->camera_transform.w[1] = RENDER_CENTER_OFFSET;

  return state;
}

void render_state_destroy(RenderState *state) {
  glfwDestroyWindow(state->window);
  free(state->swapchain_images);
  free(state->instance);
  free(state);
}

void render_state_create_window(RenderState *state) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  state->window = glfwCreateWindow((i32)state->window_width, (i32)state->window_height, "Miniflow", NULL, NULL);

  glfwSetWindowUserPointer(state->window, state);
}

u8 render_state_should_close(RenderState *state) { return glfwWindowShouldClose(state->window); }
