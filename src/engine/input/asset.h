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

/**
 * @brief Finds the real path of a file within the context of assets.
 *
 * The resulting path is relative to the current working directory.
 *
 * @param asset_path Relative asset path.
 * @param out Buffer to write the real asset path to.
 */
void asset_find(const char *asset_path, char *out);

/**
 * @brief Loads an image from file path.
 *
 * Uses asset search paths to search for the asset. Will use the
 * missing texture data (magenta-black checkerboard pattern) if
 * the image cannot be found.
 *
 * @param img The image to write to.
 * @param image_path The path to the image to load.
 */
void asset_load_image(Image *img, const char *image_path);

/**
 * @brief Frees the memory associated with the image.
 *
 * @param img The image to unload.
 */
void asset_unload_image(Image *img);

#ifdef __cplusplus
}
#endif

#endif
