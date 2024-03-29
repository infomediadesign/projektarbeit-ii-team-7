#include "render_state.h"

RenderState *render_state_init() {
  RenderState *state = (RenderState *)malloc(sizeof(RenderState));

  const Matrix4 null_mat4 = {
    { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }
  };

  state->monitor = glfwGetPrimaryMonitor();

  f32 xscale, yscale;
  glfwGetMonitorContentScale(state->monitor, &xscale, &yscale);

  state->current_frame           = 0;
  state->current_swapchain_image = 0;
  state->window_width            = 1280.0f * xscale;
  state->window_height           = 720.0f * yscale;
  state->window_scale_x          = xscale;
  state->window_scale_y          = yscale;
  state->debug                   = 0;
  state->rendering               = 0;
  state->camera_transform        = null_mat4;
  state->render_scale            = RENDER_SCALE * xscale;

  state->camera_transform.x[0] = state->render_scale;
  state->camera_transform.y[1] = 1280.0f / 720.0f * state->render_scale;
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
