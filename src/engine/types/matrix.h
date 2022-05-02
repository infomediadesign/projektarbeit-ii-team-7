#ifndef __ENGINE_TYPES_MATRIX_H
#define __ENGINE_TYPES_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "numeric.h"
#include "vector.h"

typedef struct Matrix4 {
  f32 x[4];
  f32 y[4];
  f32 z[4];
  f32 w[4];
} Matrix4;

// clang-format off

void matrix_inspect(const Matrix4 m);
void matrix_inspect_math(const Matrix4 m);
Matrix4 matrix_make(
  const f32 x1, const f32 x2, const f32 x3, const f32 x4,
  const f32 y1, const f32 y2, const f32 y3, const f32 y4,
  const f32 z1, const f32 z2, const f32 z3, const f32 z4,
  const f32 w1, const f32 w2, const f32 w3, const f32 w4
);
Matrix4 matrix_make3(
  const f32 x1, const f32 x2, const f32 x3,
  const f32 y1, const f32 y2, const f32 y3,
  const f32 z1, const f32 z2, const f32 z3
);
Matrix4 matrix_make_translation(const Vector4 v);
Matrix4 matrix_neg(const Matrix4 m);
Matrix4 matrix_mul(const Matrix4 m1, const Matrix4 m2);
Matrix4 matrix_add(const Matrix4 m1, const Matrix4 m2);
Matrix4 matrix_sub(const Matrix4 m1, const Matrix4 m2);

// clang-format on

#ifdef __cplusplus
}
#endif

#endif
