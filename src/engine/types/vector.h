#ifndef __ENGINE_TYPES_VECTOR_H
#define __ENGINE_TYPES_VECTOR_H

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

#endif
