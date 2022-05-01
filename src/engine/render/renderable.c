#include "renderable.h"
#include "../util.h"
#include <math.h>
#include <string.h>

// clang-format off
static const Vector3 one_vec3 = {1.0f, 1.0f, 1.0f};
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
  r->rotation = null_vec4;
  r->transform_matrix = null_mat4;
  r->active = GS_FALSE;
  r->velocity = null_vec3;
  r->scale = one_vec3;
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
      .usage =
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      .flags = 0,
  };

  const VkBufferCreateInfo uv_buffer_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .size = sizeof(Vector2) * r->vertices_count,
      .usage =
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
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
          state, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};

  vkGetBufferMemoryRequirements(state->device, r->uv_buffer,
                                &memory_requirements);

  const VkMemoryAllocateInfo uv_memory_alloc_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = geyser_get_memory_type_index(
          state, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};

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

  vkBindBufferMemory(state->device, r->vertex_buffer, r->vertex_memory, 0);
  vkBindBufferMemory(state->device, r->uv_buffer, r->uv_memory, 0);
}

void renderable_send_memory(RenderState *state, Renderable *r) {
  const u64 vertex_size = sizeof(Vector4) * r->vertices_count;
  const u64 uv_size = sizeof(Vector2) * r->vertices_count;
  void *data;

  VkBufferCopy copy_info = {
      .srcOffset = 0, .dstOffset = 0, .size = vertex_size};

  geyser_success_or_message(vkMapMemory(state->device, state->memory, 0,
                                        r->vertex_memory_size, 0, &data),
                            "Failed to map vertex memory!");
  memcpy(data, r->vertices, vertex_size);
  memcpy(data + vertex_size, r->uv, uv_size);
  vkUnmapMemory(state->device, state->memory);

  vkCmdCopyBuffer(state->command_buffer, state->buffer, r->vertex_buffer, 1,
                  &copy_info);

  copy_info.srcOffset = vertex_size;
  copy_info.size = uv_size;

  vkCmdCopyBuffer(state->command_buffer, state->buffer, r->uv_buffer, 1,
                  &copy_info);

  geyser_cmd_submit_staging(state);
}

void renderable_make_rect(const RenderState *state, Renderable *r,
                          const f32 width, const f32 height) {
  // clang-format off
  Vector4 vertices[6] = {
    {-1.0f,         -1.0f,          0.0f, 1.0f},
    {-1.0f,         -1.0f + height, 0.0f, 1.0f},
    {-1.0f + width, -1.0f,          0.0f, 1.0f},
    {-1.0f + width, -1.0f,          0.0f, 1.0f},
    {-1.0f,         -1.0f + height, 0.0f, 1.0f},
    {-1.0f + width, -1.0f + height, 0.0f, 1.0f}
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
  /* Translation */
  r->transform_matrix.w[0] = r->position.x;
  r->transform_matrix.w[1] = r->position.y;
  r->transform_matrix.w[2] = r->position.z;

  /* Linear scale and unit quanterion rotation */
  r->transform_matrix.x[0] = 1.0f - 2.0f * r->rotation.y * r->rotation.y -
                             2.0f * r->rotation.z * r->rotation.z;
  r->transform_matrix.x[1] = 2.0f * r->rotation.x * r->rotation.y +
                             2.0f * r->rotation.w * r->rotation.z;
  r->transform_matrix.x[2] = 2.0f * r->rotation.x * r->rotation.z -
                             2.0f * r->rotation.w * r->rotation.y;

  r->transform_matrix.y[0] = 2.0f * r->rotation.x * r->rotation.y -
                             2.0f * r->rotation.w * r->rotation.z;
  r->transform_matrix.y[1] = 1.0f - 2.0f * r->rotation.x * r->rotation.x -
                             2.0f * r->rotation.z * r->rotation.z;
  r->transform_matrix.y[2] = 2.0f * r->rotation.y * r->rotation.z -
                             2.0f * r->rotation.w * r->rotation.x;

  r->transform_matrix.z[0] = 2.0f * r->rotation.x * r->rotation.z +
                             2.0f * r->rotation.w * r->rotation.y;
  r->transform_matrix.z[1] = 2.0f * r->rotation.y * r->rotation.z +
                             2.0f * r->rotation.w * r->rotation.x;
  r->transform_matrix.z[2] = 1.0f - 2.0f * r->rotation.x * r->rotation.x -
                             2.0f * r->rotation.y * r->rotation.y;

  r->transform_matrix.x[0] *= r->scale.x;
  r->transform_matrix.y[1] *= r->scale.y;
  r->transform_matrix.z[2] *= r->scale.z;
}

void renderable_interpolate(Renderable *r) {
  const f64 delta = platform_time_f64() - r->updated_at;

  r->position.x = util_lerp_f32(delta, r->last_position.x,
                                r->last_position.x + r->velocity.x);
  r->position.y = util_lerp_f32(delta, r->last_position.y,
                                r->last_position.y + r->velocity.y);
  r->position.z = util_lerp_f32(delta, r->last_position.z,
                                r->last_position.z + r->velocity.z);
}

void renderable_set_pos(Renderable *r, const Vector4 pos) {
  r->position = pos;
  r->last_position = pos;
}

void renderable_set_rotation(Renderable *r, const Vector3 axis,
                             const f32 rotation) {
  if (!vector_is_normal3(&axis)) {
    return;
  }

  r->rotation.x = axis.x * sinf(rotation * 0.5);
  r->rotation.y = axis.y * sinf(rotation * 0.5);
  r->rotation.z = axis.z * sinf(rotation * 0.5);
  r->rotation.w = cosf(rotation * 0.5);
}

void renderable_set_velocity(Renderable *r, const Vector3 vel) {
  r->velocity = vel;
}
