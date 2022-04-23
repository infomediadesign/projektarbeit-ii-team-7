#ifndef __ENGINE_RENDER_RENDERABLE_H
#define __ENGINE_RENDER_RENDERABLE_H

#include "geyser.h"

typedef struct Matrix4 {
  float x[4];
  float y[4];
  float z[4];
  float w[4];
} Matrix4;

typedef struct Renderable {
  Vector4 *vertices;
  Vector2 *uv;
  GeyserImage texture;
  u32 vertices_count;
  Vector4 position;
  Vector4 angle;
  Matrix4 transform_matrix;

  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_memory;
  VkDeviceSize vertex_memory_size;

  VkBuffer uv_buffer;
  VkDeviceMemory uv_memory;
  VkDeviceSize uv_memory_size;
} Renderable;

Renderable renderable_default();
void renderable_allocate_memory(RenderState *state, Renderable *r);
void renderable_send_memory(RenderState *state, Renderable *r);
void renderable_make_rect(Renderable *r, const f32 x, const f32 y,
                          const f32 width, const f32 height);

#endif
