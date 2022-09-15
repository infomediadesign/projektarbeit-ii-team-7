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
  char texture_path[256];
  Matrix4 transform_matrix;
  Vector4 last_position;
  Vector4 position;
  Vector4 color;
  Quaternion rotation;
  GeyserTexture texture;
  Vector3 velocity;
  Vector2 *uv;
  Vector2 scale;
  Vector2 uv_offset;
  MemoryPool *pool;
  f64 updated_at;
  i64 assigned_to;
  u64 offset;
  u32 vertices_count;
  GeyserBool active;
  GeyserBool should_zsort;
} Renderable;

/**
 * @brief Creates a new default renderable.
 *
 * @return Renderable empty renderable.
 */
Renderable renderable_default();

/**
 * @brief Sets renderable to default values.
 *
 * Please ensure that the renderable doesn't have any memory allocated.
 * If you used it before, free it first. Otherwise it's a memory leak
 * waiting to happen.
 *
 * @param r Renderable to set to default values.
 */
void renderable_make_default(Renderable *r);

/**
 * @brief Sends renderable memory to the GPU.
 *
 * @warning Only do this in a staging pass.
 *
 * This sends the vertex and UV data to the assigned
 * memory pool slot.
 *
 * @param state The render state.
 * @param r The renderable whose memory needs to be sent to the GPU.
 */
void renderable_send_memory(RenderState *state, Renderable *r);

/**
 * @brief A helper to create rectangle vertices in a renderable.
 *
 * This creates two tris, arranged to form a rectangle of `width`x`height`.
 * The coordinate of the leftmost corner is offset by half of `width`, such
 * that the origin point (0, 0, 0) is directly in the center of the rectangle,
 * for the purposes of correctness of the rotation pivot.
 *
 * This also creates a UV (texcoord) map, which assumes that the entire texture belongs
 * to the rectangle.
 *
 * @param state The render state.
 * @param r The renderable to write to.
 */
void renderable_make_rect(const RenderState *state, Renderable *r);

/**
 * @brief A helper to create rectangle vertices in a renderable with ability to specity UVs.
 *
 * This creates two tris, arranged to form a rectangle of `width`x`height`.
 * The coordinate of the leftmost corner is offset by half of `width`, such
 * that the origin point (0, 0, 0) is directly in the center of the rectangle,
 * for the purposes of correctness of the rotation pivot.
 *
 * This also creates a UV (texcoord) map based on `uv_height` and `uv_width` variables. The UVs
 * are relative to upper-left corner of the texture. Use `uv_offset` of the renderable to translate
 * the resulting UV rectangle.
 *
 * @param state The render state.
 * @param r The renderable to write to.
 * @param uv_width The width of the UV rectangle, between 0 and 1, representing percentage of the texture width.
 * @param uv_height The width of the UV rectangle, between 0 and 1, representing percentage of the texture height.
 */
void renderable_make_rect_ex(const RenderState *state, Renderable *r, const f32 uv_width, const f32 uv_height);

/**
 * @brief Marks the memory of the renderable as free, and deallocates buffers.
 *
 * @warning Only do this in a staging pass.
 *
 * @param state The render state.
 * @param r The renderable to free the memory of.
 */
void renderable_free(RenderState *state, Renderable *r);

/**
 * @brief Calculates the transform matrix of the renderable.
 *
 * Scales, rotates, transforms.
 *
 * @param r The renderable to calculate the matrix of.
 */
void renderable_calc_matrix(Renderable *r);

/**
 * @brief Interpolates the movement of the renderable.
 *
 * This is designed to provide smooth continuous motion at <rendesh rate>hz,
 * even when logic is lagging behind or running at a significantly slower
 * pace than the renderer.
 *
 * This is essentially a function of momentum and time. Uses the last time of
 * when the renderable was updated by the logic as pivot point for interp
 * calculations.
 *
 * @param r The renderable to interpolate.
 */
void renderable_interpolate(Renderable *r);

/**
 * @brief Sets the position of the renderable.
 *
 * Also sets various helper variables to help the interpolation logic.
 *
 * @param r The renderable.
 * @param pos New position, in meters.
 */
void renderable_set_pos(Renderable *r, const Vector4 pos);

/**
 * @brief Sets the scale of the renderable.
 *
 * Where 1.0 is 100%, or the original size.
 *
 * @param r The renderable.
 * @param scale The scale of the renderable.
 */
void renderable_set_scale(Renderable *r, const Vector2 scale);

/**
 * @brief Sets whether the renderable is active or not.
 *
 * Inactive renderables won't be rendered.
 *
 * @param r The renderable.
 * @param active GS_TRUE or GS_FALSE
 */
void renderable_set_active(Renderable *r, GeyserBool active);

/**
 * @brief Sets which logical entity ID the renderable is assigned to.
 *
 * @param r The renderable.
 * @param ent_id The entity ID.
 */
void renderable_set_assigned(Renderable *r, i64 ent_id);

/**
 * @brief Sets the rotation of the renderable.
 *
 * Uses the pivot axis, and rotation around that axis. The axis must
 * be a normal vector. Uses quaternions.
 *
 * @param r The renderable.
 * @param axis Axis, around which to rotate.
 * @param rotation Amount of radians to rotate by.
 */
void renderable_set_rotation(Renderable *r, const Vector3 axis, const f32 rotation);

/**
 * @brief Sets velocity of the renderable.
 *
 * This is pivotal part of the interpolation calculations. This must be set,
 * otherwise interpolation will be incorrect.
 *
 * @param r The renderable.
 * @param vel The current velocity, in meters per second.
 */
void renderable_set_velocity(Renderable *r, const Vector3 vel);

/**
 * @brief Loads renderable texture from path.
 *
 * @param state The render state.
 * @param r The renderable.
 * @param image_path Path to the image relative to the asset search paths.
 */
void renderable_load_texture(RenderState *state, Renderable *r, const char *image_path);

/**
 * @brief Sets renderable texture from a pre-loaded image.
 *
 * @param state The render state.
 * @param r The renderable.
 * @param tex_img The texture image data to set the texture to.
 */
void renderable_set_texture(RenderState *state, Renderable *r, const Image tex_img);

/**
 * @brief Sets the offset of the texture coordinates (UVs).
 *
 * @param r The renderable.
 * @param offset The offset (in Vector2 of floats between 0 and 1, representing percentage) of the UV coordinates.
 */
void renderable_set_uv_offset(Renderable *r, const Vector2 offset);

/**
 * @brief Helper to initialize a renderable as a rectangle.
 *
 * @param state The render state.
 * @param r The renderable.
 */
void renderable_init_rect(RenderState *state, Renderable *r);

/**
 * @brief Helper to initialize a renderable as a rectangle.
 *
 * @param state The render state.
 * @param r The renderable.
 * @param uv_width The width of the UV rectangle, between 0 and 1, representing percentage of the texture width.
 * @param uv_height The width of the UV rectangle, between 0 and 1, representing percentage of the texture height.
 */
void renderable_init_rect_ex(RenderState *state, Renderable *r, const f32 uv_width, const f32 uv_height);

/**
 * @brief Sets the last time of the logic update.
 *
 * This is necessary for the correct interpolation of movement of the renderable.
 *
 * @param r The renderable.
 * @param updated_at The time of the last logic update.
 */
void renderable_set_updated(Renderable *r, const f64 updated_at);

/**
 * @brief Sets whether or not the renderable should be Z-sorted.
 *
 * Z-sorted renderables should always be drawn after non-sorted ones.
 *
 * @param r The renderable.
 * @param should_zsort Whether or not the renderable should be Z-sorted.
 */
void renderable_set_should_zsort(Renderable *r, const GeyserBool should_zsort);

/**
 * @brief Gets the size, in bytes, of the renderable vertex and UV buffers.
 *
 * @param r The renderable.
 * @return u64 The size, in bytes, of the vertex and UV buffers.
 */
u64 renderable_get_size(const Renderable *r);

/**
 * @brief Assigns renderable memory from the memory pool.
 *
 * @param state The render state.
 * @param m The memory manager to assign the memory from.
 * @param r The renderable.
 */
void renderable_assign_memory(RenderState *state, MemoryManager *m, Renderable *r);

#ifdef __cplusplus
}
#endif

#endif
