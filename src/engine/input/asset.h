#ifndef __ENGINE_INPUT_ASSET_H
#define __ENGINE_INPUT_ASSET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types.h"

typedef struct Image {
  u32 *data;
  u32 height;
  u32 width;
} Image;

Image asset_load_image(const char *image_path);

#ifdef __cplusplus
}
#endif

#endif
