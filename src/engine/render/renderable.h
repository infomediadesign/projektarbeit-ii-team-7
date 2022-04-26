#ifndef __ENGINE_RENDER_RENDERABLE_H
#define __ENGINE_RENDER_RENDERABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/matrix.h"
#include "geyser.h"
#include "render_state.h"

typedef struct Renderable {
  Vector4 *vertices;
  Vector2 *uv;
  GeyserSamplerView texture;
  u32 vertices_count;
  Vector4 position;
  Vector4 angle;
  Matrix4 transform_matrix;
  GeyserBool active;

  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_memory;
  VkDeviceSize vertex_memory_size;

  VkBuffer uv_buffer;
  VkDeviceMemory uv_memory;
  VkDeviceSize uv_memory_size;
} Renderable;

Renderable renderable_default();
void renderable_make_default(Renderable *r);
void renderable_load_texture(RenderState *state, Renderable *r);
void renderable_allocate_memory(RenderState *state, Renderable *r);
void renderable_send_memory(RenderState *state, Renderable *r);
void renderable_make_rect(const RenderState *state, Renderable *r, const f32 x,
                          const f32 y, const f32 width, const f32 height);
void renderable_free(const RenderState *state, Renderable *r);

#ifdef __cplusplus
}
#endif

#endif
