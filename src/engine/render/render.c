#include "render.h"

void glfw_error_fun(int error_code, const char *error_message) {
  printf("[GLFW Error] %s\n", error_message);
}

int render_perform(void *args) {
  ThreadData *const td = (ThreadData *)args;
  GameState *const state = (GameState *)td->state;
  RenderState *const render_state = render_state_init();

  glfwSetErrorCallback(glfw_error_fun);

  if (!glfwInit()) {
    game_add_flag(state, GS_EXIT);

    return 1;
  }

  if (glfwVulkanSupported() == GLFW_TRUE) {
    DEBUG_MESSAGE("Vulkan is supported\n");
  } else {
    DEBUG_MESSAGE("Vulkan is not supported\n");

    glfwTerminate();
    game_add_flag(state, GS_EXIT);

    return 1;
  }

  render_state_create_window(render_state);
  geyser_init_vk(render_state);

  while (!game_should_exit(state)) {
    if (render_state_should_close(render_state)) {
      game_add_flag(state, GS_EXIT);
      break;
    }
  }

  geyser_destroy_vk(render_state);
  render_state_free(render_state);
  glfwTerminate();

  return 0;
}
