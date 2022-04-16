#ifndef __ENGINE_RENDER_STATE_H
#define __ENGINE_RENDER_STATE_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include "../platform.h"
#include "../state/state.h"
#include "../util.h"
#include <GLFW/glfw3.h>

typedef struct RenderState {
  GLFWwindow *window;

  VkInstance *instance;
  VkPhysicalDevice *physical_device;
  VkPhysicalDeviceFeatures *physical_device_features;
  VkDevice *device;
  VkQueue *queue;
  VkSurfaceKHR *surface;
  VkSwapchainKHR *swapchain;
  VkImageView *image_view;
  VkFramebuffer *framebuffer;
  VkPipeline *pipeline;
  VkCommandBuffer *command_buffer;
  VkCommandPool *command_pool;
} RenderState;

RenderState *render_state_init();
void render_state_free(RenderState *state);
void render_state_create_window(RenderState *state);
u8 render_state_should_close(RenderState *state);

#endif
