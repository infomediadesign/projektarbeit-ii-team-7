#ifndef __ENGINE_RENDER_STATE_H
#define __ENGINE_RENDER_STATE_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include "../platform.h"
#include "../state/state.h"
#include "../util.h"
#include <glad/vulkan.h>
#include <GLFW/glfw3.h>

/**
 * @brief Stores all the information necessary for rendering.
 *
 * This includes all vulkan structures, the GLFW window, as well
 * as other misc. variables.
 *
 */
typedef struct RenderState {
  GLFWwindow *window;

  /* Persistent stuff */
  VkInstance instance;
  VkPhysicalDevice physical_device;
  VkPhysicalDeviceFeatures physical_device_features;
  VkDevice device;
  VkQueue queue;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkImage backbuffer_image;
  VkImageView backbuffer;
  VkRenderPass renderpass;
  VkFramebuffer framebuffer;
  VkPipeline pipeline;
  VkCommandBuffer command_buffer;
  VkCommandPool command_pool;
  VkPhysicalDeviceMemoryProperties memory_properties;

  /* Variable stuff */
  VkImage *swapchain_images;
  VkSemaphore semaphore;
  u32 current_swapchain_image;
  u64 current_frame;
  VkFormat preferred_color_format;
} RenderState;

/**
 * @brief Initializes a new render state.
 *
 * @return RenderState* Pointer to a newly allocated RenderState.
 */
RenderState *render_state_init();

/**
 * @brief Frees up render state resources.
 *
 * @param state State to be destroyed.
 */
void render_state_destroy(RenderState *state);

/**
 * @brief Spawns a new GLFW window.
 *
 * ```
 * render_state_create_window(render_state);
 * do_something_with_window(render_state->window);
 * ```
 *
 * @param state Render state to spawn the window in.
 */
void render_state_create_window(RenderState *state);

/**
 * @brief Determines if the window should close or not.
 *
 * Please note that this only accounts for user clicking the
 * close button in the corner of the window and doesn't actually
 * have any idea about the state of the rest of things.
 *
 * @param state Render state to check.
 * @return u8 Whether or not the window should close.
 */
u8 render_state_should_close(RenderState *state);

#endif
