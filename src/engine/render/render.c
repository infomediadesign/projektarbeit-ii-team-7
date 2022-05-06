#include "render.h"

#include "shaders/shaders.h"

#include <game/interface.h>

#define MAX_RENDERABLES 16384

void glfw_error_fun(int error_code, const char *error_message) {
  printf("\033[1;31m[GLFW Error]\033[0m %s\n", error_message);
}

static inline const char *platform_name(i32 platform) {
  switch (platform) {
  case GLFW_PLATFORM_WAYLAND: return "Wayland"; break;
  case GLFW_PLATFORM_X11: return "X11"; break;
  case GLFW_PLATFORM_COCOA: return "Cocoa"; break;
  case GLFW_PLATFORM_WIN32: return "Win32"; break;
  default: return "Unknown";
  }
}

int render_perform(void *args) {
  ThreadData *const td            = (ThreadData *)args;
  mutex_t *lock                   = (mutex_t *)td->lock;
  GameState *const state          = (GameState *)td->state;
  RenderState *const render_state = render_state_init();
  Renderable *const renderables   = (Renderable *)calloc(MAX_RENDERABLES, sizeof(Renderable));

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
    free(renderables);
    render_state_destroy(render_state);

    return 1;
  }

  if (glfwVulkanSupported() == GLFW_TRUE) {
    DEBUG_MESSAGE("Vulkan is supported\n");
  } else {
    DEBUG_MESSAGE("Vulkan is not supported\n");

    glfwTerminate();
    game_add_flag(state, GS_EXIT);
    free(renderables);
    render_state_destroy(render_state);

    return 1;
  }

  printf("[GLFW] Selected platform: %s\n", platform_name(glfwGetPlatform()));

  for (u32 i = 0; i < MAX_RENDERABLES; i++)
    renderable_make_default(&renderables[i]);

  render_state_create_window(render_state);
  geyser_init_vk(render_state);

  state->window = render_state->window;

  geyser_cmd_begin_staging(render_state);

  const VkDescriptorSetLayoutBinding descriptor_bindings[] = {
    { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL }
  };

  const VkPushConstantRange push_constant_range[] = {
    { VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GeyserPushConstants) },
  };

  GeyserVertexInputDescription vertex_input_description = geyser_create_vertex_input_description();

  geyser_add_vertex_input_binding(&vertex_input_description, 0, 16, VK_VERTEX_INPUT_RATE_VERTEX);
  geyser_add_vertex_input_binding(&vertex_input_description, 1, 8, VK_VERTEX_INPUT_RATE_VERTEX);

  geyser_add_vertex_input_attribute(&vertex_input_description, 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
  geyser_add_vertex_input_attribute(&vertex_input_description, 1, 1, VK_FORMAT_R32G32_SFLOAT, 0);

  geyser_create_pipeline(
    render_state,
    descriptor_bindings,
    1,
    push_constant_range,
    1,
    unlit_generic_vert_data,
    unlit_generic_vert_data_size,
    unlit_generic_frag_data,
    unlit_generic_frag_data_size,
    &vertex_input_description,
    (GeyserPipeline *)&render_state->pipeline
  );

  geyser_cmd_end_staging(render_state);

  u64 delay = state->fps_max != 0 ? 1000000 / state->fps_max : 0;
  i64 start_time, end_time;
  u64 avg                       = 0;
  const VkDeviceSize offsets[1] = { 0 };

  render_state->init_time = platform_time();

  /* Actual rendering loop */
  while (!game_should_exit(state)) {
    if (render_state_should_close(render_state)) {
      game_add_flag(state, GS_EXIT);
      break;
    }

    start_time = platform_time_usec();

    geyser_cmd_begin_staging(render_state);

    game_adjust_renderables(state, lock, render_state, renderables, MAX_RENDERABLES);

    geyser_cmd_end_staging(render_state);

    render_state->rendering = 1;

    geyser_cmd_begin_draw(render_state);
    geyser_cmd_begin_renderpass(render_state);

    vkCmdBindPipeline(render_state->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_state->pipeline.pipeline);

    geyser_cmd_set_viewport(render_state);

    GeyserPushConstants push_constants = { .camera = render_state->camera_transform };

    for (u32 i = 0; i < MAX_RENDERABLES; i++) {
      if (renderables[i].active == GS_TRUE && renderables[i].vertices_count > 0) {
        renderable_interpolate(&renderables[i]);

        push_constants.quaternion   = quaternion_to_vec(renderables[i].rotation);
        push_constants.position     = renderables[i].position;
        push_constants.vertex_color = renderables[i].color;
        push_constants.scale        = renderables[i].scale;
        push_constants.uv_offset    = renderables[i].uv_offset;

        if (i != 0 && renderables[i].vertices_count == renderables[i - 1].vertices_count) {
          if (memcmp(renderables[i].vertex_buffer, renderables[i - 1].vertex_buffer, sizeof(renderables[i].vertex_buffer)) != 0)
            vkCmdBindVertexBuffers(render_state->command_buffer, 0, 1, &renderables[i].vertex_buffer, offsets);
          if (memcmp(renderables[i].uv_buffer, renderables[i - 1].uv_buffer, sizeof(renderables[i].uv_buffer)) != 0)
            vkCmdBindVertexBuffers(render_state->command_buffer, 1, 1, &renderables[i].uv_buffer, offsets);
        } else {
          vkCmdBindVertexBuffers(render_state->command_buffer, 0, 1, &renderables[i].vertex_buffer, offsets);
          vkCmdBindVertexBuffers(render_state->command_buffer, 1, 1, &renderables[i].uv_buffer, offsets);
        }

        vkCmdBindDescriptorSets(
          render_state->command_buffer,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          render_state->pipeline.pipeline_layout,
          0,
          1,
          &renderables[i].texture.descriptor_set,
          0,
          NULL
        );
        vkCmdPushConstants(
          render_state->command_buffer,
          render_state->pipeline.pipeline_layout,
          VK_SHADER_STAGE_VERTEX_BIT,
          0,
          sizeof(GeyserPushConstants),
          &push_constants
        );

        vkCmdDraw(render_state->command_buffer, renderables[i].vertices_count, 1, 0, 0);
      }
    }

    geyser_cmd_end_renderpass(render_state);
    geyser_cmd_end_draw(render_state);

    vkDeviceWaitIdle(render_state->device);

    render_state->rendering = 0;
    render_state->current_frame++;

    end_time = platform_time_usec();

    avg += (end_time - start_time);
    avg = avg >> 1;

    printf("\ravg %lu fps %lu    ", avg, 1000000 / avg);

    if (state->fps_max > 0 && (end_time - start_time) < delay)
      platform_usleep(delay - (end_time - start_time));
  }

  for (u32 i = 0; i < MAX_RENDERABLES; i++)
    renderable_free(render_state, &renderables[i]);

  free(renderables);
  geyser_destroy_vk(render_state);
  render_state_destroy(render_state);
  glfwTerminate();

  return 0;
}
