#include "asset.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#include <stb_image.h>

Image asset_load_image(const char *image_path) {
  Image img = {.data = NULL};
  i32 image_width, image_height, _n;

  u8 *data = stbi_load(image_path, &image_width, &image_height, &_n, 4);

  if (data == NULL) {
    return img;
  }

  img.data = (u32 *)calloc(image_width * image_height * 4, sizeof(u8));
  img.width = image_width;
  img.height = image_height;

  memcpy(img.data, data, image_width * image_height * 4);

  free(data);

  return img;
}
