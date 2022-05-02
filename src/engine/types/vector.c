#include "vector.h"
#include <math.h>

Vector2 vector_default2() { return (Vector2){0.0f, 0.0f}; }

Vector3 vector_default3() { return (Vector3){0.0f, 0.0f, 0.0f}; }

Vector4 vector_default4() { return (Vector4){0.0f, 0.0f, 0.0f, 0.0f}; }

Vector2 vector_make2(const f32 x, const f32 y) { return (Vector2){x, y}; }

Vector3 vector_make3(const f32 x, const f32 y, const f32 z) {
  return (Vector3){x, y, z};
}

Vector4 vector_make4(const f32 x, const f32 y, const f32 z, const f32 w) {
  return (Vector4){x, y, z, w};
}

Vector3 vector_add3(const Vector3 v1, const Vector3 v2) {
  return (Vector3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

void vector_add3i(Vector3 *v1, const Vector3 v2) {
  v1->x += v2.x;
  v1->y += v2.y;
  v1->z += v2.z;
}

Vector3 vector_sub3(const Vector3 v1, const Vector3 v2) {
  return (Vector3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

void vector_sub3i(Vector3 *v1, const Vector3 v2) {
  v1->x -= v2.x;
  v1->y -= v2.y;
  v1->z -= v2.z;
}

Vector3 vector_scale3(const Vector3 v1, const f32 scale) {
  return (Vector3){v1.x * scale, v1.y * scale, v1.z * scale};
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
  return (Vector3){v.x / len, v.y / len, v.z / len};
}

void vector_normal3i(Vector3 *v) {
  const f32 len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

  v->x /= len;
  v->y /= len;
  v->z /= len;
}

Vector4 vector_sub4(const Vector4 v1, const Vector4 v2) {
  return (Vector4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

void vector_sub4i(Vector4 *v1, const Vector4 v2) {
  v1->x -= v2.x;
  v1->y -= v2.y;
  v1->z -= v2.z;
  v1->w -= v2.w;
}
