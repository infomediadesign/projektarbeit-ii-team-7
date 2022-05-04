#include "matrix.h"

#include <stdio.h>

// clang-format off

void matrix_inspect(const Matrix4 m) {
  printf(
    "Matrix4 {\n  x: %5.2f %5.2f %5.2f %5.2f\n  y: %5.2f %5.2f %5.2f %5.2f\n  z: %5.2f %5.2f %5.2f %5.2f\n  w: %5.2f %5.2f %5.2f %5.2f\n}\n",
    m.x[0], m.x[1], m.x[2], m.x[3],
    m.y[0], m.y[1], m.y[2], m.y[3],
    m.z[0], m.z[1], m.z[2], m.z[3],
    m.w[0], m.w[1], m.w[2], m.w[3]
  );
}

void matrix_inspect_math(const Matrix4 m) {
  printf(
    "    x     y     z     w\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n",
    m.x[0], m.y[0], m.z[0], m.w[0],
    m.x[1], m.y[1], m.z[1], m.w[1],
    m.x[2], m.y[2], m.z[2], m.w[2],
    m.x[3], m.y[3], m.z[3], m.w[3]
  );
}

Matrix4 matrix_make(
  const f32 x1, const f32 x2, const f32 x3, const f32 x4,
  const f32 y1, const f32 y2, const f32 y3, const f32 y4,
  const f32 z1, const f32 z2, const f32 z3, const f32 z4,
  const f32 w1, const f32 w2, const f32 w3, const f32 w4
) {
  return (Matrix4) {
    .x = {x1, x2, x3, x4},
    .y = {y1, y2, y3, y4},
    .z = {z1, z2, z3, z4},
    .w = {w1, w2, w3, w4}
  };
}

Matrix4 matrix_make3(
  const f32 x1, const f32 x2, const f32 x3,
  const f32 y1, const f32 y2, const f32 y3,
  const f32 z1, const f32 z2, const f32 z3
) {
  return matrix_make(
    x1,   x2,   x3,   0.0f,
    y1,   y2,   y3,   0.0f,
    z1,   z2,   z3,   0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
}

Matrix4 matrix_make_translation(const Vector4 v) {
  return matrix_make(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
     v.x,  v.y,  v.z, 1.0f
  );
}

Matrix4 matrix_mul(const Matrix4 m1, const Matrix4 m2) {
  return matrix_make(
    /* x */
    m1.x[0] * m2.x[0] + m1.x[1] * m2.y[0] + m1.x[2] * m2.z[0] + m1.x[3] * m2.w[0],
    m1.y[0] * m2.x[0] + m1.y[1] * m2.y[0] + m1.y[2] * m2.z[0] + m1.y[3] * m2.w[0],
    m1.z[0] * m2.x[0] + m1.z[1] * m2.y[0] + m1.z[2] * m2.z[0] + m1.z[3] * m2.w[0],
    m1.w[0] * m2.x[0] + m1.w[1] * m2.y[0] + m1.w[2] * m2.z[0] + m1.w[3] * m2.w[0],
    /* y */
    m1.x[0] * m2.x[1] + m1.x[1] * m2.y[1] + m1.x[2] * m2.z[1] + m1.x[3] * m2.w[1],
    m1.y[0] * m2.x[1] + m1.y[1] * m2.y[1] + m1.y[2] * m2.z[1] + m1.y[3] * m2.w[1],
    m1.z[0] * m2.x[1] + m1.z[1] * m2.y[1] + m1.z[2] * m2.z[1] + m1.z[3] * m2.w[1],
    m1.w[0] * m2.x[1] + m1.w[1] * m2.y[1] + m1.w[2] * m2.z[1] + m1.w[3] * m2.w[1],
    /* z */
    m1.x[0] * m2.x[2] + m1.x[1] * m2.y[2] + m1.x[2] * m2.z[2] + m1.x[3] * m2.w[2],
    m1.y[0] * m2.x[2] + m1.y[1] * m2.y[2] + m1.y[2] * m2.z[2] + m1.y[3] * m2.w[2],
    m1.z[0] * m2.x[2] + m1.z[1] * m2.y[2] + m1.z[2] * m2.z[2] + m1.z[3] * m2.w[2],
    m1.w[0] * m2.x[2] + m1.w[1] * m2.y[2] + m1.w[2] * m2.z[2] + m1.w[3] * m2.w[2],
    /* w */
    m1.x[0] * m2.x[3] + m1.x[1] * m2.y[3] + m1.x[2] * m2.z[3] + m1.x[3] * m2.w[3],
    m1.y[0] * m2.x[3] + m1.y[1] * m2.y[3] + m1.y[2] * m2.z[3] + m1.y[3] * m2.w[3],
    m1.z[0] * m2.x[3] + m1.z[1] * m2.y[3] + m1.z[2] * m2.z[3] + m1.z[3] * m2.w[3],
    m1.w[0] * m2.x[3] + m1.w[1] * m2.y[3] + m1.w[2] * m2.z[3] + m1.w[3] * m2.w[3]
  );
}

Matrix4 matrix_neg(const Matrix4 m) {
  return (Matrix4) {
    .x = {-m.x[0], -m.x[1], -m.x[2], -m.x[3]},
    .y = {-m.y[0], -m.y[1], -m.y[2], -m.y[3]},
    .z = {-m.z[0], -m.z[1], -m.z[2], -m.z[3]},
    .w = {-m.w[0], -m.w[1], -m.w[2], -m.w[3]}
  };
}

Matrix4 matrix_add(const Matrix4 m1, const Matrix4 m2) {
  return (Matrix4) {
    .x = {m1.x[0] + m2.x[0], m1.x[1] + m2.x[1], m1.x[2] + m2.x[2], m1.x[3] + m2.x[3]},
    .y = {m1.y[0] + m2.y[0], m1.y[1] + m2.y[1], m1.y[2] + m2.y[2], m1.y[3] + m2.y[3]},
    .z = {m1.z[0] + m2.z[0], m1.z[1] + m2.z[1], m1.z[2] + m2.z[2], m1.z[3] + m2.z[3]},
    .w = {m1.w[0] + m2.w[0], m1.w[1] + m2.w[1], m1.w[2] + m2.w[2], m1.w[3] + m2.w[3]},
  };
}

Matrix4 matrix_sub(const Matrix4 m1, const Matrix4 m2) {
  return (Matrix4) {
    .x = {m1.x[0] - m2.x[0], m1.x[1] - m2.x[1], m1.x[2] - m2.x[2], m1.x[3] - m2.x[3]},
    .y = {m1.y[0] - m2.y[0], m1.y[1] - m2.y[1], m1.y[2] - m2.y[2], m1.y[3] - m2.y[3]},
    .z = {m1.z[0] - m2.z[0], m1.z[1] - m2.z[1], m1.z[2] - m2.z[2], m1.z[3] - m2.z[3]},
    .w = {m1.w[0] - m2.w[0], m1.w[1] - m2.w[1], m1.w[2] - m2.w[2], m1.w[3] - m2.w[3]},
  };
}

// clang-format on
