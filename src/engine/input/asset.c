#include "asset.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#include <stb_image.h>

void flip_channels(u32 *data, const u32 size) {
  for (u32 i = 0; i < size; i++) {
    data[i] = (data[i] & 0xff00ff00) | ((data[i] & 0x00ff0000) >> 16) |
              ((data[i] & 0x000000ff) << 16);
  }
}

Image asset_load_image(const char *image_path) {
  Image img = {.data = NULL};
  i32 image_width, image_height, _n;

  u8 *data = stbi_load(image_path, &image_width, &image_height, &_n, 4);

  if (data == NULL) {
    return img;
  }

  const u32 size = image_width * image_height * 4;

  img.data = (u32 *)calloc(size, sizeof(u8));
  img.width = image_width;
  img.height = image_height;

  memcpy(img.data, data, size);

  free(data);

  flip_channels(img.data, image_width * image_height);

  return img;
}
