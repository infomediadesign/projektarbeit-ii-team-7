#ifndef __ENGINE_TYPES_VECTOR_H
#define __ENGINE_TYPES_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types.h"

typedef struct Vector2 {
  f32 x;
  f32 y;
} Vector2;

typedef struct Vector3 {
  f32 x;
  f32 y;
  f32 z;
} Vector3;

typedef struct Vector4 {
  f32 x;
  f32 y;
  f32 z;
  f32 w;
} Vector4;

Vector2 vector_default2();
Vector3 vector_default3();
Vector4 vector_default4();
Vector4 vector_make4(const f32 x, const f32 y, const f32 z, const f32 w);
Vector3 vector_make3(const f32 x, const f32 y, const f32 z);
Vector2 vector_make2(const f32 x, const f32 y);
Vector3 vector_add3(const Vector3 v1, const Vector3 v2);
Vector3 vector_sub3(const Vector3 v1, const Vector3 v2);
Vector3 vector_scale3(const Vector3 v1, const f32 scale);
void vector_add3i(Vector3 *v1, const Vector3 v2);
void vector_sub3i(Vector3 *v1, const Vector3 v2);
void vector_scale3i(Vector3 *v1, const f32 scale);

#ifdef __cplusplus
}
#endif

#endif
