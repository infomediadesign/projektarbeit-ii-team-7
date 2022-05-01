#ifndef __ENGINE_CRC64_H
#define __ENGINE_CRC64_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/numeric.h"

u64 crc64(const void *restrict p, const size_t len);

#ifdef __cplusplus
}
#endif

#endif
