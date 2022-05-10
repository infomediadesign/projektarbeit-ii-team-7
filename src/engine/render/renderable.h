#ifndef __ENGINE_RENDER_RENDERABLE_H
#define __ENGINE_RENDER_RENDERABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/matrix.h"
#include "../types/numeric.h"
#include "../types/quaternion.h"
#include "../types/vector.h"
#include "geyser.h"
#include "memory.h"
#include "render_state.h"

typedef struct Renderable {
  Matrix4 transform_matrix;
  Vector4 last_position;
  Vector4 position;
  Vector4 color;
  Quaternion rotation;
  GeyserTexture texture;
  Vector3 velocity;
  Vector4 *vertices;
  Vector2 *uv;
  Vector2 scale;
  Vector2 uv_offset;
  MemoryPool *pool;
  f64 updated_at;
  i64 assigned_to;
  u64 offset;
  u32 vertices_count;
  GeyserBool active;
} Renderable;

Renderable renderable_default();
void renderable_make_default(Renderable *r);
void renderable_send_memory(RenderState *state, Renderable *r);
void renderable_make_rect(const RenderState *state, Renderable *r, const f32 width, const f32 height);
void renderable_free(const RenderState *state, Renderable *r);
void renderable_calc_matrix(Renderable *r);
void renderable_interpolate(Renderable *r);
void renderable_set_pos(Renderable *r, const Vector4 pos);
void renderable_set_scale(Renderable *r, const Vector2 scale);
void renderable_set_active(Renderable *r, GeyserBool active);
void renderable_set_assigned(Renderable *r, i64 ent_id);
void renderable_set_rotation(Renderable *r, const Vector3 axis, const f32 rotation);
void renderable_set_velocity(Renderable *r, const Vector3 vel);
void renderable_load_texture(RenderState *state, Renderable *r, const char *image_path);
void renderable_init_rect(RenderState *state, Renderable *r, const f32 width, const f32 height);
void renderable_set_updated(Renderable *r, const f64 updated_at);
u64 renderable_get_size(const Renderable *r);
void renderable_assign_memory(RenderState *state, MemoryManager *m, Renderable *r);

#ifdef __cplusplus
}
#endif

#endif
