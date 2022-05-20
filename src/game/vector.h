#ifndef __GAME_VECTOR_H
#define __GAME_VECTOR_H

#include <engine/types/vector.h>

/* Vector2 */

inline bool operator==(const Vector2 &v1, const Vector2 &v2);
inline bool operator!=(const Vector2 &v1, const Vector2 &v2);

Vector2 operator+(const Vector2 &v1, const Vector2 &v2);
Vector2 operator-(const Vector2 &v1, const Vector2 &v2);
Vector2 &operator+=(Vector2 &v1, const Vector2 &v2);
Vector2 &operator-=(Vector2 &v1, const Vector2 &v2);

template<typename T>
Vector2 operator/(const Vector2 &v, T s) {
  return { v.x / (f32)s, v.y / (f32)s };
}

template<typename T>
Vector2 operator*(const Vector2 &v, T s) {
  return { v.x * (f32)s, v.y * (f32)s };
}

template<typename T>
Vector2 &operator/=(Vector2 &v, T s) {
  v.x /= (f32)s;
  v.y /= (f32)s;

  return v;
}

template<typename T>
Vector2 &operator*=(Vector2 &v, T s) {
  v.x *= (f32)s;
  v.y *= (f32)s;

  return v;
}

/* Vector3 */

inline bool operator==(const Vector3 &v1, const Vector3 &v2);
inline bool operator!=(const Vector3 &v1, const Vector3 &v2);

Vector3 operator+(const Vector3 &v1, const Vector3 &v2);
Vector3 operator-(const Vector3 &v1, const Vector3 &v2);
Vector3 &operator+=(Vector3 &v1, const Vector3 &v2);
Vector3 &operator-=(Vector3 &v1, const Vector3 &v2);

template<typename T>
Vector3 operator/(const Vector3 &v, T s) {
  return { v.x / (f32)s, v.y / (f32)s, v.z / (f32)s };
}

template<typename T>
Vector3 operator*(const Vector3 &v, T s) {
  return { v.x * (f32)s, v.y * (f32)s, v.z * (f32)s };
}

template<typename T>
Vector3 &operator/=(Vector3 &v, T s) {
  v.x /= (f32)s;
  v.y /= (f32)s;
  v.z /= (f32)s;

  return v;
}

template<typename T>
Vector3 &operator*=(Vector3 &v, T s) {
  v.x *= (f32)s;
  v.y *= (f32)s;
  v.z *= (f32)s;

  return v;
}

/* Vector4 */

inline bool operator==(const Vector4 &v1, const Vector4 &v2);
inline bool operator!=(const Vector4 &v1, const Vector4 &v2);

Vector4 operator+(const Vector4 &v1, const Vector4 &v2);
Vector4 operator-(const Vector4 &v1, const Vector4 &v2);
Vector4 &operator+=(Vector4 &v1, const Vector4 &v2);
Vector4 &operator-=(Vector4 &v1, const Vector4 &v2);

template<typename T>
Vector4 operator/(const Vector4 &v, T s) {
  return { v.x / (f32)s, v.y / (f32)s, v.z / (f32)s, v.w / (f32)s };
}

template<typename T>
Vector4 operator*(const Vector4 &v, T s) {
  return { v.x * (f32)s, v.y * (f32)s, v.z * (f32)s, v.w * (f32)s };
}

template<typename T>
Vector4 &operator/=(Vector4 &v, T s) {
  v.x /= (f32)s;
  v.y /= (f32)s;
  v.z /= (f32)s;
  v.w /= (f32)s;

  return v;
}

template<typename T>
Vector4 &operator*=(Vector4 &v, T s) {
  v.x *= (f32)s;
  v.y *= (f32)s;
  v.z *= (f32)s;
  v.w *= (f32)s;

  return v;
}

#endif
