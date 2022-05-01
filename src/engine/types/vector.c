#include "vector.h"
#include <math.h>

Vector2 vector_default2() {
  Vector2 vec = {.x = 0.0f, .y = 0.0f};
  return vec;
}

Vector3 vector_default3() {
  Vector3 vec = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
  return vec;
}

Vector4 vector_default4() {
  Vector4 vec = {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f};
  return vec;
}

Vector4 vector_make4(const f32 x, const f32 y, const f32 z, const f32 w) {
  Vector4 vec = {x, y, z, w};
  return vec;
}

Vector3 vector_make3(const f32 x, const f32 y, const f32 z) {
  Vector3 vec = {x, y, z};
  return vec;
}

Vector2 vector_make2(const f32 x, const f32 y) {
  Vector2 vec = {x, y};
  return vec;
}

Vector3 vector_add3(const Vector3 v1, const Vector3 v2) {
  return vector_make3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

void vector_add3i(Vector3 *v1, const Vector3 v2) {
  v1->x += v2.x;
  v1->y += v2.y;
  v1->z += v2.z;
}

Vector3 vector_sub3(const Vector3 v1, const Vector3 v2) {
  return vector_make3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

void vector_sub3i(Vector3 *v1, const Vector3 v2) {
  v1->x -= v2.x;
  v1->y -= v2.y;
  v1->z -= v2.z;
}

Vector3 vector_scale3(const Vector3 v1, const f32 scale) {
  return vector_make3(v1.x * scale, v1.y * scale, v1.z * scale);
}

void vector_scale3i(Vector3 *v1, const f32 scale) {
  v1->x *= scale;
  v1->y *= scale;
  v1->z *= scale;
}

f32 vector_length3(const Vector3 v) {
  return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

u8 vector_is_normal3(const Vector3 *v) {
  return v->x >= -1.0f && v->x <= 1.0f && v->y >= -1.0f && v->y <= 1.0f &&
         v->z >= -1.0f && v->z <= 1.0f;
}

Vector3 vector_normal3(const Vector3 v) {
  const f32 len = vector_length3(v);
  return vector_make3(v.x / len, v.y / len, v.z / len);
}

void vector_normal3i(Vector3 *v) {
  const f32 len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

  v->x /= len;
  v->y /= len;
  v->z /= len;
}
