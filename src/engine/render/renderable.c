#include "renderable.h"
#include <string.h>

// clang-format off
static const Vector4 null_vec4 = {0.0f, 0.0f, 0.0f, 1.0f};
static const Matrix4 null_mat4 = {
  {1.0f, 0.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 0.0f, 1.0f}
};
// clang-format on

void renderable_make_default(Renderable *r) {
  r->vertices = NULL;
  r->uv = NULL;
  r->vertices_count = 0U;
  r->position = null_vec4;
  r->angle = null_vec4;
  r->transform_matrix = null_mat4;
  r->active = GS_FALSE;
}

Renderable renderable_default() {
  Renderable r;

  renderable_make_default(&r);

  return r;
}

void renderable_allocate_memory(RenderState *state, Renderable *r) {
  VkMemoryRequirements memory_requirements;

  const VkBufferCreateInfo vertex_buffer_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .size = sizeof(Vector4) * r->vertices_count,
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .flags = 0,
  };

  const VkBufferCreateInfo uv_buffer_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .size = sizeof(Vector2) * r->vertices_count,
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .flags = 0,
  };

  geyser_success_or_message(vkCreateBuffer(state->device, &vertex_buffer_info,
                                           NULL, &r->vertex_buffer),
                            "Failed to create vertex buffer!");
  geyser_success_or_message(
      vkCreateBuffer(state->device, &uv_buffer_info, NULL, &r->uv_buffer),
      "Failed to create UV buffer!");

  vkGetBufferMemoryRequirements(state->device, r->vertex_buffer,
                                &memory_requirements);

  const VkMemoryAllocateInfo vertex_memory_alloc_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = geyser_get_memory_type_index(
          state, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)};

  vkGetBufferMemoryRequirements(state->device, r->uv_buffer,
                                &memory_requirements);

  const VkMemoryAllocateInfo uv_memory_alloc_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = geyser_get_memory_type_index(
          state, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)};

  geyser_success_or_message(vkAllocateMemory(state->device,
                                             &vertex_memory_alloc_info, NULL,
                                             &r->vertex_memory),
                            "Failed to allocate vertex buffer memory!");
  geyser_success_or_message(vkAllocateMemory(state->device,
                                             &uv_memory_alloc_info, NULL,
                                             &r->uv_memory),
                            "Failed to allocate UV buffer memory!");

  r->vertex_memory_size = vertex_memory_alloc_info.allocationSize;
  r->uv_memory_size = uv_memory_alloc_info.allocationSize;
}

void renderable_send_memory(RenderState *state, Renderable *r) {
  void *data;

  geyser_success_or_message(vkMapMemory(state->device, r->vertex_memory, 0,
                                        r->vertex_memory_size, 0, &data),
                            "Failed to map vertex memory!");
  memcpy(data, r->vertices, sizeof(Vector4) * r->vertices_count);
  vkUnmapMemory(state->device, r->vertex_memory);

  geyser_success_or_message(
      vkMapMemory(state->device, r->uv_memory, 0, r->uv_memory_size, 0, &data),
      "Failed to map UV memory!");
  memcpy(data, r->uv, sizeof(Vector2) * r->vertices_count);
  vkUnmapMemory(state->device, r->uv_memory);

  vkBindBufferMemory(state->device, r->vertex_buffer, r->vertex_memory, 0);
  vkBindBufferMemory(state->device, r->uv_buffer, r->uv_memory, 0);
}

void renderable_make_rect(const RenderState *state, Renderable *r, const f32 x,
                          const f32 y, const f32 width, const f32 height) {
  // clang-format off
  Vector4 vertices[6] = {
    {x / state->window_width,            y / state->window_height,           0.0f, 1.0f},
    {x / state->window_width,           (y + height) / state->window_height, 0.0f, 1.0f},
    {(x + width) / state->window_width,  y / state->window_height,           0.0f, 1.0f},
    {(x + width) / state->window_width,  y / state->window_height,           0.0f, 1.0f},
    {x / state->window_width,           (y + height) / state->window_height, 0.0f, 1.0f},
    {(x + width) / state->window_width, (y + height) / state->window_height, 0.0f, 1.0f}
  };

  Vector2 uvmap[6] = {
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f}
  };
  // clang-format on

  r->vertices = malloc(sizeof(Vector4) * 6);
  r->uv = malloc(sizeof(Vector2) * 6);

  memcpy(r->vertices, vertices, sizeof(Vector4) * 6);
  memcpy(r->uv, uvmap, sizeof(Vector2) * 6);

  r->vertices_count = 6U;
}

void renderable_free(const RenderState *state, Renderable *r) {
  vkFreeMemory(state->device, r->vertex_memory, NULL);
  vkFreeMemory(state->device, r->uv_memory, NULL);
}
