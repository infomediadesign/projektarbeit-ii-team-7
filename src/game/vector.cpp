#include "vector.h"

/* Vector2 */

inline bool operator==(const Vector2 &v1, const Vector2 &v2) { return v1.x == v2.x && v1.y == v2.y; }

inline bool operator!=(const Vector2 &v1, const Vector2 &v2) { return v1.x != v2.x || v1.y != v2.y; }

Vector2 operator+(const Vector2 &v1, const Vector2 &v2) { return { v1.x + v2.x, v1.y + v2.y }; }

Vector2 operator-(const Vector2 &v1, const Vector2 &v2) { return { v1.x - v2.x, v1.y - v2.y }; }

Vector2 &operator+=(Vector2 &v1, const Vector2 &v2) {
  v1.x += v2.x;
  v1.y += v2.y;

  return v1;
}

Vector2 &operator-=(Vector2 &v1, const Vector2 &v2) {
  v1.x -= v2.x;
  v1.y -= v2.y;

  return v1;
}

/* Vector3 */

inline bool operator==(const Vector3 &v1, const Vector3 &v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z; }

inline bool operator!=(const Vector3 &v1, const Vector3 &v2) { return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z; }

Vector3 operator+(const Vector3 &v1, const Vector3 &v2) { return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; }

Vector3 operator-(const Vector3 &v1, const Vector3 &v2) { return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z }; }

Vector3 &operator+=(Vector3 &v1, const Vector3 &v2) {
  v1.x += v2.x;
  v1.y += v2.y;
  v1.z += v2.z;

  return v1;
}

Vector3 &operator-=(Vector3 &v1, const Vector3 &v2) {
  v1.x -= v2.x;
  v1.y -= v2.y;
  v1.z -= v2.z;

  return v1;
}

/* Vector4 */

inline bool operator==(const Vector4 &v1, const Vector4 &v2) {
  return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

inline bool operator!=(const Vector4 &v1, const Vector4 &v2) {
  return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
}

Vector4 operator+(const Vector4 &v1, const Vector4 &v2) {
  return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}

Vector4 operator-(const Vector4 &v1, const Vector4 &v2) {
  return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

Vector4 &operator+=(Vector4 &v1, const Vector4 &v2) {
  v1.x += v2.x;
  v1.y += v2.y;
  v1.z += v2.z;
  v1.w += v2.w;

  return v1;
}

Vector4 &operator-=(Vector4 &v1, const Vector4 &v2) {
  v1.x -= v2.x;
  v1.y -= v2.y;
  v1.z -= v2.z;
  v1.w -= v2.w;

  return v1;
}
