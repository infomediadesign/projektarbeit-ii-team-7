#ifndef __ENGINE_RENDER_RENDER_STATE_H
#define __ENGINE_RENDER_RENDER_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

// clang-format off
#include "../platform.h"
#include "../state/state.h"
#include "../util.h"
#include "../types/matrix.h"
#include <glad/vulkan.h>
#include <GLFW/glfw3.h>

// clang-format on

typedef struct BackbufferView {
  VkImage image;
  void *pool;
  u64 offset;
  VkImageView view;
} BackbufferView;

/* Copy of GeyserPipeline */
typedef struct RsPipeline {
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkShaderModule vertex_shader;
  VkShaderModule fragment_shader;
  VkPipeline pipeline;
} RsPipeline;

typedef struct RsMemoryManager {
  void *pools;
} RsMemoryManager;

/**
 * @brief Stores all the information necessary for rendering.
 *
 * This includes all vulkan structures, the GLFW window, as well
 * as other misc. variables.
 *
 */
typedef struct RenderState {
  Matrix4 camera_transform;
  RsPipeline pipeline;
  BackbufferView backbuffer;
  Time init_time;
  u64 current_frame;
  GLFWwindow *window;
  u32 *queue_family_indices;
  VkImage *swapchain_images;
  RsMemoryManager *memory_manager;
  VkInstance instance;
  VkPhysicalDevice physical_device;
  VkPhysicalDeviceFeatures physical_device_features;
  VkPhysicalDeviceProperties physical_device_properties;
  VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
  VkDevice device;
  VkQueue queue;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkRenderPass renderpass;
  VkFramebuffer framebuffer;
  VkDescriptorPool descriptor_pool;
  VkCommandPool command_pool;
  VkBuffer buffer;
  VkDeviceMemory memory;
  VkViewport viewport;
  VkCommandBuffer command_buffer;
  VkSemaphore image_semaphore;
  VkSemaphore draw_semaphore;
  VkFormat preferred_color_format;
  VkImageMemoryBarrier pre_draw_barrier;
  VkImageMemoryBarrier pre_present_barrier;
  VkRect2D scissor;
  f32 window_width;
  f32 window_height;
  u32 current_swapchain_image;
  u32 queue_family_index;
  u32 queue_family_indices_count;
  u32 swapchain_image_count;
  u8 debug;
  u8 rendering;
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

#ifdef __cplusplus
}
#endif

#endif
