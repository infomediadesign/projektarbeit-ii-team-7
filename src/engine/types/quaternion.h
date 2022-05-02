#ifndef __ENGINE_TYPES_QUATERNION_H
#define __ENGINE_TYPES_QUATERNION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "matrix.h"
#include "numeric.h"
#include "vector.h"

typedef struct Quaternion {
  f32 x;
  f32 y;
  f32 z;
  f32 w;
} Quaternion;

Quaternion quaternion_make(const f32 x, const f32 y, const f32 z, const f32 w);
Quaternion quaternion_from_vec(const Vector4 v);
Quaternion quaternion_rotation(const Vector3 axis, const f32 angle);
Quaternion quaternion_normal(const Quaternion q);
void quaternion_normalize(Quaternion *q);
u8 quaternion_is_normal(const Quaternion *q);
Quaternion quaternion_mul(const Quaternion q1, const Quaternion q2);
void quaternion_muli(Quaternion *q1, const Quaternion q2);
Matrix4 quaternion_rotation_matrix(const Quaternion q);

#ifdef __cplusplus
}
#endif

#endif
