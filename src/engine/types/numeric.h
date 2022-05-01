#ifndef __ENGINE_TYPES_H
#define __ENGINE_TYPES_H

#ifdef __cplusplus
#define RESTRICTED_PTR *
extern "C" {
#else
#define RESTRICTED_PTR *restrict
#endif

#define MF_PI 3.14159265359

#include <stdint.h>
#include <stdlib.h>

/* Integers */
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

/* Floats */
typedef float f32;
typedef double f64;

/* Callbacks */
typedef int (*ThreadCallback)(void *);

#ifdef __cplusplus
}
#endif

#endif
