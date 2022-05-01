#ifndef __ENGINE_TYPES_MATRIX_H
#define __ENGINE_TYPES_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "numeric.h"

typedef struct Matrix3 {
  f32 x[3];
  f32 y[3];
  f32 z[3];
} Matrix3;

typedef struct Matrix4 {
  f32 x[4];
  f32 y[4];
  f32 z[4];
  f32 w[4];
} Matrix4;

#ifdef __cplusplus
}
#endif

#endif
