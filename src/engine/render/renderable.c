#include "renderable.h"
#include "../util.h"
#include <string.h>

// clang-format off
static const Vector3 null_vec3 = {0.0f, 0.0f, 0.0f};
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
  r->last_position = null_vec4;
  r->angle = null_vec4;
  r->transform_matrix = null_mat4;
  r->active = GS_FALSE;
  r->velocity = null_vec3;
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
  const f32 half_width = state->window_width * 0.5f;
  const f32 half_height = state->window_height * 0.5f;

  // clang-format off
  Vector4 vertices[6] = {
    {-1.0f + (x / half_width),           -1.0f + (y / half_height),            0.0f, 1.0f},
    {-1.0f + (x / half_width),           -1.0f + ((y + height) / half_height), 0.0f, 1.0f},
    {-1.0f + ((x + width) / half_width), -1.0f + (y / half_height),            0.0f, 1.0f},
    {-1.0f + ((x + width) / half_width), -1.0f + (y / half_height),            0.0f, 1.0f},
    {-1.0f + (x / half_width),           -1.0f + ((y + height) / half_height), 0.0f, 1.0f},
    {-1.0f + ((x + width) / half_width), -1.0f + ((y + height) / half_height), 0.0f, 1.0f}
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

void renderable_calc_matrix(Renderable *r) {
  /* The constants are the value of 1 pixel at 768x432 */
  r->transform_matrix.w[0] = r->position.x * 0.0026f;
  r->transform_matrix.w[1] = r->position.y * 0.0046f;
  r->transform_matrix.w[2] = r->position.z;
}

void renderable_interpolate(Renderable *r) {
  const f64 current_time = platform_time_f64();

  r->position.x =
      util_lerp_f32(current_time - r->updated_at, r->last_position.x,
                    r->last_position.x + r->velocity.x);
  r->position.y =
      util_lerp_f32(current_time - r->updated_at, r->last_position.y,
                    r->last_position.y + r->velocity.y);
}

void renderable_set_pos(Renderable *r, const Vector4 pos) {
  r->position = pos;
  r->last_position = pos;
}

void renderable_set_velocity(Renderable *r, const Vector3 vel) {
  r->velocity = vel;
}
