#include "quaternion.h"
#include <math.h>

Quaternion quaternion_make(const f32 x, const f32 y, const f32 z, const f32 w) {
  return (Quaternion){x, y, z, w};
}

Quaternion quaternion_from_vec(const Vector4 v) {
  return (Quaternion){v.x, v.y, v.z, v.w};
}

Quaternion quaternion_rotation(const Vector3 axis, const f32 rotation) {
  return (Quaternion){axis.x * sinf(rotation * 0.5),
                      axis.y * sinf(rotation * 0.5),
                      axis.z * sinf(rotation * 0.5), cosf(rotation * 0.5)};
}

Quaternion quaternion_normal(const Quaternion q) {
  const f32 len = sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);

  return (Quaternion){q.x / len, q.y / len, q.z / len, q.w / len};
}

void quaternion_normalize(Quaternion *q) {
  const f32 len = sqrt(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);

  q->x /= len;
  q->y /= len;
  q->z /= len;
  q->w /= len;
}

u8 quaternion_is_normal(const Quaternion *q) {
  return q->x >= -1.0f && q->x <= 1.0f && q->y >= -1.0f && q->y <= 1.0f &&
         q->z >= -1.0f && q->z <= 1.0f && q->w >= -1.0f && q->w <= 1.0f;
}

Quaternion quaternion_mul(const Quaternion q1, const Quaternion q2) {
  return (Quaternion){q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
                      q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
                      q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
                      q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z};
}

void quaternion_muli(Quaternion *q1, const Quaternion q2) {
  const Quaternion q = quaternion_mul(*q1, q2);

  q1->x = q.x;
  q1->y = q.y;
  q1->z = q.z;
  q1->w = q.w;
}

// clang-format off

Matrix4 quaternion_rotation_matrix(const Quaternion q) {
  return matrix_make3(
    /* x */
    1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z,
    2.0f * q.x * q.y + 2.0f * q.w * q.z,
    2.0f * q.x * q.z - 2.0f * q.w * q.y,
    /* y */
    2.0f * q.x * q.y -2.0f * q.w * q.z,
    1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z,
    2.0f * q.y * q.z -2.0f * q.w * q.x,
    /* z */
    2.0f * q.x * q.z + 2.0f * q.w * q.y,
    2.0f * q.y * q.z + 2.0f * q.w * q.x,
    1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y
  );
}

// clang-format on
