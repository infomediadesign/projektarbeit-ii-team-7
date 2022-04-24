#ifndef __ENGINE_RENDER_GEYSER_H
#define __ENGINE_RENDER_GEYSER_H

#ifdef __cplusplus
extern "C" {
#endif

#define GEYSER_MINIMAL_VK_STRUCT_INFO(t) .sType = t, .pNext = NULL
#define GEYSER_BASIC_VK_STRUCT_INFO(t) .sType = t, .pNext = NULL, .flags = 0

/**
 * Geyser is a minimalistic Vulkan middleware library.
 *
 * It is domain-specific and isn't designed to be used
 * anywhere outside of this project, however it could be
 * used with some adaptation.
 *
 */
#include "../types/vector.h"
#include "render_state.h"

typedef struct GeyserImage {
  VkImage image;
  VkDeviceMemory memory;
} GeyserImage;

typedef struct GeyserImageView {
  VkImageView view;
  VkImage image;
  VkDeviceMemory memory;
} GeyserImageView;

typedef struct GeyserPipeline {
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkShaderModule vertex_shader;
  VkShaderModule fragment_shader;
  VkPipeline pipeline;
} GeyserPipeline;

typedef struct GeyserVertexInputDescription {
  u32 input_binding_description_size;
  u32 input_attribute_description_size;
  VkVertexInputBindingDescription input_binding_descriptions[16];
  VkVertexInputAttributeDescription input_attribute_descriptions[16];
} GeyserVertexInputDescription;

/**
 * @brief Initializes Vulkan resources.
 *
 * Populates most Vulkan-related fields in the
 * RenderState struct. It also creates the window surface
 * for rendering.
 *
 * ```
 * geyser_init_vk(render_state);
 * ...
 * // do some rendering here
 * ...
 * geyser_destroy_vk(render_state);
 * ```
 *
 * @warning Will crash the app if Vulkan fails to initialize.
 * @param state Rendering state.
 */
void geyser_init_vk(RenderState *state);

/**
 * @brief Frees Vulkan resources.
 *
 * ```
 * // some rendering here
 * geyser_destroy_vk(render_state);
 * // you can't do vulkan rendering past this point
 * ```
 *
 * @param state Rendering state
 */
void geyser_destroy_vk(RenderState *restrict state);

void geyser_success_or_message(const VkResult res, const char *message);
void geyser_fill_image_view_creation_structs(
    RenderState *state, VkImageSubresourceRange *resource_range,
    VkComponentMapping *mapping, VkImageViewCreateInfo *creation_info);
GeyserImageView *geyser_create_image_view(RenderState *state,
                                          const Vector2 size,
                                          VkImageViewType type);
GeyserImageView *geyser_create_image_view_from_image(RenderState *state,
                                                     VkImage *img,
                                                     VkImageViewType type);
u32 geyser_get_memory_type_index(const RenderState *restrict state,
                                 const VkMemoryPropertyFlagBits flag);
GeyserImage *geyser_create_image(const RenderState *restrict state,
                                 const Vector2 size);
void geyser_allocate_image_memory(const RenderState *restrict state,
                                  GeyserImage *image);
GeyserImage *geyser_create_and_allocate_image(const RenderState *restrict state,
                                              const Vector2 size);
GeyserPipeline *geyser_create_pipeline(
    const RenderState *restrict state,
    const VkDescriptorSetLayoutBinding descriptor_bindings[],
    const u32 descriptor_bindings_size,
    const VkPushConstantRange push_constant_ranges[],
    const u32 push_constant_ranges_size, const u8 vertex_shader_data[],
    const u32 vertex_shader_data_size, const u8 fragment_shader_data[],
    const u32 fragment_shader_data_size,
    GeyserVertexInputDescription *vertex_input_description);
GeyserVertexInputDescription geyser_create_vertex_input_description();
void geyser_add_vertex_input_binding(GeyserVertexInputDescription *description,
                                     const u32 binding, const u32 stride,
                                     const VkVertexInputRate input_rate);
void geyser_add_vertex_input_attribute(
    GeyserVertexInputDescription *description, const u32 location,
    const u32 binding, VkFormat format, const u32 offset);

void geyser_cmd_begin_draw(RenderState *restrict state);
void geyser_cmd_end_draw(RenderState *restrict state);
void geyser_create_semaphore(const RenderState *restrict state,
                             VkSemaphore *semaphore);
void geyser_cmd_begin_renderpass(const RenderState *restrict state);
void geyser_cmd_end_renderpass(const RenderState *restrict state);
void geyser_cmd_set_viewport(const RenderState *restrict state);

#ifdef __cplusplus
}
#endif

#endif
