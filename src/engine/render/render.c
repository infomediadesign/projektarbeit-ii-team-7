#include "render.h"
#include "shaders/shaders.h"

void glfw_error_fun(int error_code, const char *error_message) {
  printf("\033[1;31m[GLFW Error]\033[0m %s\n", error_message);
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

  const VkDescriptorSetLayoutBinding descriptor_bindings[] = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL_GRAPHICS,
       NULL}};

  const VkPushConstantRange push_constant_range[] = {
      {VK_SHADER_STAGE_ALL_GRAPHICS, 0, 4}};

  GeyserVertexInputDescription vertex_input_description =
      geyser_create_vertex_input_description(1U);
  geyser_add_vertex_input_binding(&vertex_input_description, 0, 16,
                                  VK_VERTEX_INPUT_RATE_VERTEX);
  geyser_add_vertex_input_attribute(&vertex_input_description, 0, 0,
                                    VK_FORMAT_R32G32B32A32_SFLOAT, 0);

  GeyserPipeline *pipeline3d = geyser_create_pipeline(
      render_state, descriptor_bindings, 1, push_constant_range, 1,
      unlit_generic_vert_data, unlit_generic_vert_data_size,
      unlit_generic_frag_data, unlit_generic_frag_data_size,
      &vertex_input_description);

  render_state->pipeline3d = pipeline3d->pipeline;

  u64 delay = 1000000 / state->fps_max;
  i64 start_time, end_time;
  u64 avg = 0;

  render_state->init_time = platform_time();

  /* Actual rendering loop */
  while (!game_should_exit(state)) {
    if (render_state_should_close(render_state)) {
      game_add_flag(state, GS_EXIT);
      break;
    }

    start_time = platform_time_usec();

    glfwPollEvents();

    geyser_cmd_begin_draw(render_state);
    geyser_cmd_begin_renderpass(render_state);

    geyser_cmd_end_renderpass(render_state);
    geyser_cmd_end_draw(render_state);

    vkDeviceWaitIdle(render_state->device);

    render_state->current_frame++;

    end_time = platform_time_usec();

    avg += (end_time - start_time);
    avg = avg >> 1;

    printf("\ravg %lu fps %lu    ", avg, 1000000 / avg);

    if (state->fps_max > 0 && (end_time - start_time) < delay)
      platform_usleep(delay - (end_time - start_time));
  }

  free(pipeline3d);
  geyser_destroy_vk(render_state);
  render_state_destroy(render_state);
  glfwTerminate();

  return 0;
}
