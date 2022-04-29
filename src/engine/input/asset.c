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

void asset_load_image(Image *img, const char *image_path) {
  img->data = NULL;
  i32 image_width, image_height, _n;

  u8 *data =
      stbi_load(image_path, &image_width, &image_height, &_n, STBI_rgb_alpha);

  if (data == NULL) {
    abort();
  }

  const u32 size = image_width * image_height * 4;

  img->data = (u32 *)data;
  img->width = image_width;
  img->height = image_height;

  // flip_channels(img->data, image_width * image_height);
}

void asset_unload_image(Image *img) { stbi_image_free(img->data); }
