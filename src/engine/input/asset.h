#ifndef __ENGINE_INPUT_ASSET_H
#define __ENGINE_INPUT_ASSET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/numeric.h"

typedef struct Image {
  u32 *data;
  u32 height;
  u32 width;
} Image;

void asset_load_image(Image *img, const char *image_path);
void asset_unload_image(Image *img);

#ifdef __cplusplus
}
#endif

#endif
