#include "render.h"
#include "shaders/shaders.h"

#include <game/game.h>

#define MAX_RENDERABLES 2048

void glfw_error_fun(int error_code, const char *error_message) {
  printf("\033[1;31m[GLFW Error]\033[0m %s\n", error_message);
}

static inline const char *platform_name(i32 platform) {
  switch (platform) {
  case GLFW_PLATFORM_WAYLAND:
    return "Wayland";
    break;
  case GLFW_PLATFORM_X11:
    return "X11";
    break;
  case GLFW_PLATFORM_COCOA:
    return "Cocoa";
    break;
  case GLFW_PLATFORM_WIN32:
    return "Win32";
    break;
  default:
    return "Unknown";
  }
}

int render_perform(void *args) {
  ThreadData *const td = (ThreadData *)args;
  mutex_t *lock = (mutex_t *)td->lock;
  GameState *const state = (GameState *)td->state;
  RenderState *const render_state = render_state_init();
  Renderable *const renderables =
      (Renderable *)calloc(MAX_RENDERABLES, sizeof(Renderable));

  if (game_is_debug(state))
    render_state->debug = 1;

  glfwSetErrorCallback(glfw_error_fun);

#ifndef _WIN32
  if (strcmp(state->preferred_platform, "x11") == 0)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
  else
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
#endif

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

  printf("[GLFW] Selected platform: %s\n", platform_name(glfwGetPlatform()));

  render_state_create_window(render_state);
  geyser_init_vk(render_state);

  const VkDescriptorSetLayoutBinding descriptor_bindings[] = {
      {0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
       VK_SHADER_STAGE_FRAGMENT_BIT, NULL}};

  const VkPushConstantRange push_constant_range[] = {
      {VK_SHADER_STAGE_ALL_GRAPHICS, 0, 4}};

  GeyserVertexInputDescription vertex_input_description =
      geyser_create_vertex_input_description();

  geyser_add_vertex_input_binding(&vertex_input_description, 0, 16,
                                  VK_VERTEX_INPUT_RATE_VERTEX);
  geyser_add_vertex_input_binding(&vertex_input_description, 1, 8,
                                  VK_VERTEX_INPUT_RATE_VERTEX);

  geyser_add_vertex_input_attribute(&vertex_input_description, 0, 0,
                                    VK_FORMAT_R32G32B32A32_SFLOAT, 0);
  geyser_add_vertex_input_attribute(&vertex_input_description, 1, 1,
                                    VK_FORMAT_R32G32_SFLOAT, 0);

  GeyserPipeline *pipeline3d = geyser_create_pipeline(
      render_state, descriptor_bindings, 1, push_constant_range, 1,
      unlit_generic_vert_data, unlit_generic_vert_data_size,
      unlit_generic_frag_data, unlit_generic_frag_data_size,
      &vertex_input_description);

  render_state->pipeline3d = pipeline3d->pipeline;

  renderable_make_default(&renderables[0]);
  renderable_make_default(&renderables[1]);
  renderable_make_rect(render_state, &renderables[0], 32,
                       render_state->window_height - 96, 64, 64);
  renderable_make_rect(render_state, &renderables[1], 32, 32,
                       render_state->window_width - 64, 256);
  renderable_allocate_memory(render_state, &renderables[0]);
  renderable_allocate_memory(render_state, &renderables[1]);
  renderable_send_memory(render_state, &renderables[0]);
  renderable_send_memory(render_state, &renderables[1]);

  renderables[0].active = GS_TRUE;
  renderables[1].active = GS_TRUE;

  /* texture test */

  u32 test_tex[8] = {0xff0000ff, 0xff00ff00, 0xffff0000, 0xffffff00,
                     0xffff00ff, 0xff00ffff, 0xffffffff, 0xff7f7f7f};

  Image test_img = {.data = test_tex, .width = 4, .height = 2};

  const Vector2 test_img_size = {4, 2};
  GeyserTexture *tex = geyser_create_texture(render_state, test_img_size);

  geyser_set_image_memory(render_state, &tex->base.base, &test_img);
  geyser_allocate_texture_descriptor_set(render_state, tex, pipeline3d);
  geyser_update_texture_descriptor_set(render_state, tex);

  /* end texture test */

  u64 delay = 1000000 / state->fps_max;
  i64 start_time, end_time;
  u64 avg = 0;
  const VkDeviceSize offsets[1] = {0};

  render_state->init_time = platform_time();

  /* Actual rendering loop */
  while (!game_should_exit(state)) {
    if (render_state_should_close(render_state)) {
      game_add_flag(state, GS_EXIT);
      break;
    }

    start_time = platform_time_usec();

    glfwPollEvents();

    game_adjust_renderables(state, lock, renderables, MAX_RENDERABLES);

    geyser_cmd_begin_draw(render_state);
    geyser_cmd_begin_renderpass(render_state);

    vkCmdBindPipeline(render_state->command_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline3d->pipeline);

    geyser_cmd_set_viewport(render_state);

    for (u32 i = 0; i < MAX_RENDERABLES; i++) {
      if (renderables[i].active == GS_TRUE &&
          renderables[i].vertices_count > 0) {
        vkCmdBindVertexBuffers(render_state->command_buffer, 0, 1,
                               &renderables[i].vertex_buffer, offsets);
        vkCmdBindVertexBuffers(render_state->command_buffer, 1, 1,
                               &renderables[i].uv_buffer, offsets);
        vkCmdBindDescriptorSets(
            render_state->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline3d->pipeline_layout, 0, 1, &tex->descriptor_set, 0, NULL);

        vkCmdDraw(render_state->command_buffer, renderables[i].vertices_count,
                  1, 0, 0);
      }
    }

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

  for (u32 i = 0; i < MAX_RENDERABLES; i++) {
    renderable_free(render_state, &renderables[i]);
  }

  free(renderables);
  free(pipeline3d);
  geyser_destroy_vk(render_state);
  render_state_destroy(render_state);
  glfwTerminate();

  return 0;
}
